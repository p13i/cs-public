#!/usr/bin/env python3
import argparse
import os
import re
import sys
from pathlib import Path

USING_LINE_RE = re.compile(r"^\s*using\s+([:\w]+)\s*;\s*$")

# Updated: also strip an inline trailing comment after the closing brace,
# e.g., "}  // namespace", plus the following newline if present.
ANON_NAMESPACE_RE = re.compile(
    r"namespace\s*\{\s*(?P<body>(?s:.*?))\s*\}\s*(?://\s*namespace[^\n]*)?\n?",
    re.MULTILINE,
)


def extract_usings(block: str) -> dict:
    m = {}
    for line in block.splitlines():
        m2 = USING_LINE_RE.match(line)
        if not m2:
            continue
        path = m2.group(1)
        path = re.sub(r"^::", "", path)  # drop any leading ::
        short = path.split("::")[-1]
        if short:
            m[short] = path
    return m


def apply_inlines(src: str) -> str:
    # Collect all anonymous namespace blocks & remove them,
    # accumulating any simple `using ns::...::Name;`.
    replacements = {}

    def _collect_and_remove(match: re.Match) -> str:
        body = match.group("body")
        replacements.update(extract_usings(body))
        return ""  # strip the entire anonymous namespace + trailing comment

    stripped = ANON_NAMESPACE_RE.sub(_collect_and_remove, src)

    if not replacements:
        return stripped  # nothing to inline

    # Replace unqualified tokens with fully-qualified paths,
    # but do NOT touch already-qualified occurrences (guard with (?<!::)).
    # Replace longer identifiers first to avoid partial overlaps.
    for short in sorted(replacements.keys(), key=len, reverse=True):
        fq = replacements[short]
        pattern = re.compile(rf"(?<!::)\b{re.escape(short)}\b")
        stripped = pattern.sub(fq, stripped)

    return stripped


def process_file(path: Path) -> bool:
    text = path.read_text(encoding="utf-8")
    updated = apply_inlines(text)
    if updated != text:
        path.write_text(updated, encoding="utf-8")
        return True
    return False


def main():
    ap = argparse.ArgumentParser(
        description="Inline anonymous-namespace `using` declarations across all .hh and .h files."
    )
    ap.add_argument(
        "--repo_root",
        required=True,
        help="Absolute path to the repo root (e.g., passed via $(shell pwd)).",
    )
    args = ap.parse_args()
    root = Path(args.repo_root).resolve()
    if not root.exists():
        print(f"[inline_using] repo root does not exist: {root}", file=sys.stderr)
        sys.exit(2)

    changed = 0
    scanned = 0
    for dirpath, _, filenames in os.walk(root):
        for name in filenames:
            if not name.endswith(".hh") and not name.endswith(".h"):
                continue
            p = Path(dirpath) / name
            scanned += 1
            try:
                if process_file(p):
                    changed += 1
                    print(f"[inline_using] updated: {p.relative_to(root)}")
            except Exception as e:
                print(f"[inline_using] ERROR processing {p}: {e}", file=sys.stderr)
                sys.exit(3)

    print(f"[inline_using] scanned {scanned} .hh files, changed {changed}.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
