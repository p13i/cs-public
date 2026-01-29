#!/usr/bin/env python3
# cs/devtools/check_visibility.py
"""
Ensure every BUILD/BUILD.bazel file contains a canonical package line:

    package(default_visibility = ["//visibility:public"])

Behavior:
- If the file already has one or more `package(...)` stanzas, we DO NOT move them.
  We only strip any `default_visibility=[...]` occurrences from those stanzas
  (except leaving the canonical package stanza untouched if it matches exactly).

- If the file has NO `package(` at all, we INSERT a canonical package line
  *after all contiguous top-of-file `load(...)` lines*, and add exactly one
  blank line after it before the remainder of the file.

- Writes are performed ONLY if the Git working tree is clean (unless --allow-dirty).
"""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from pathlib import Path
from typing import Iterable, List, Tuple

CANONICAL = 'package(default_visibility = ["//visibility:public"])'

PACKAGE_RE = re.compile(
    r"""
        ^\s*package\s*\(          # package(
        (?P<body>.*?)             # body ... non-greedy across lines
        \)                        # )
        (?P<trail>[ \t]*\#[^\n]*)?  # optional trailing same-line comment
    """,
    re.DOTALL | re.VERBOSE | re.MULTILINE,
)

# Match top-of-file load() lines (contiguous block).
LOAD_LINE_RE = re.compile(r"^(?:\s*load\s*\([^)]*\)\s*\n)+", re.MULTILINE)

# Matches default_visibility = [ ... ] with optional surrounding commas/whitespace.
# We'll apply multiple passes to handle "first/middle/last arg" placement.
DV_PATTERNS = [
    # default_visibility first with trailing comma
    re.compile(r"""(?s)\bdefault_visibility\s*=\s*\[[^\]]*\]\s*,\s*"""),
    # default_visibility last with leading comma
    re.compile(r"""(?s)\s*,\s*default_visibility\s*=\s*\[[^\]]*\]\s*"""),
    # default_visibility only (no commas)
    re.compile(r"""(?s)\bdefault_visibility\s*=\s*\[[^\]]*\]\s*"""),
]


def run(cmd: List[str], cwd: Path | None = None) -> Tuple[int, str]:
    p = subprocess.run(
        cmd,
        cwd=str(cwd) if cwd else None,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    return p.returncode, p.stdout


def git_root(start: Path) -> Path:
    code, out = run(["git", "rev-parse", "--show-toplevel"], cwd=start)
    if code != 0:
        print(
            "ERROR: Not inside a Git repository (git rev-parse failed).",
            file=sys.stderr,
        )
        sys.exit(2)
    return Path(out.strip())


def git_clean(repo: Path) -> bool:
    code, out = run(["git", "status", "--porcelain"], cwd=repo)
    if code != 0:
        print("ERROR: git status failed.", file=sys.stderr)
        sys.exit(2)
    return out.strip() == ""


def find_build_files(root: Path) -> Iterable[Path]:
    ignore_dirs = {".git"}

    # Ignore Bazel output/external dirs seen in many workspaces.
    # If you have a .bazelignore, prefer to put these there too.
    def should_skip(dname: str) -> bool:
        return (
            dname in ignore_dirs
            or dname.startswith("bazel-")
            or dname == "external"
            or dname == ".cache"
            or dname == ".venv"
            or dname == "__pycache__"
        )

    for dirpath, dirnames, filenames in os.walk(root):
        # prune in-place
        dirnames[:] = [d for d in dirnames if not should_skip(d)]
        for fname in filenames:
            if fname == "BUILD" or fname == "BUILD.bazel":
                yield Path(dirpath) / fname


def has_any_package(content: str) -> bool:
    return re.search(r"^\s*package\s*\(", content, flags=re.MULTILINE) is not None


def insert_canonical_after_loads(content: str) -> Tuple[str, bool]:
    """
    If there is NO package() in the file, insert the canonical package line
    immediately after all contiguous top-of-file load(...) lines. Ensure exactly
    one blank line after the inserted package line before the remaining content.

    Returns (new_content, changed?)
    """
    if has_any_package(content):
        return content, False

    # Find contiguous load(...) block at the very top.
    m = LOAD_LINE_RE.match(content)
    insert_pos = m.end() if m else 0

    before = content[:insert_pos]
    after = content[insert_pos:]

    # Normalize the boundary: we want exactly one blank line after the new package line.
    after_stripped = after.lstrip("\n")
    new_content = before + CANONICAL + "\n\n" + after_stripped
    return new_content, True


def strip_default_visibility(body: str) -> str:
    new_body = body
    for pat in DV_PATTERNS:
        new_body = pat.sub("", new_body)
    # Clean up possible ", )" -> ")"
    new_body = re.sub(r"\s*,\s*\Z", "", new_body.strip(), flags=re.DOTALL)
    # Preserve internal formatting/newlines otherwise
    return new_body


def rewrite_packages(
    content: str, skip_first_if_canonical: bool = True
) -> Tuple[str, bool]:
    """
    Remove default_visibility from all package() calls except a leading canonical
    one if present exactly (and only) as CANONICAL.
    """
    changed = False
    seen_first = False

    def _repl(m: re.Match) -> str:
        nonlocal changed, seen_first
        full = m.group(0)
        body = m.group("body")
        trail = m.group("trail") or ""

        if skip_first_if_canonical and not seen_first:
            # Is this exact canonical, alone, no comment?
            if full.strip() == CANONICAL:
                seen_first = True
                return full  # leave untouched
            else:
                seen_first = True  # still mark the first package() as seen

        # Remove only default_visibility=[...] from this package() call
        new_body = strip_default_visibility(body)
        if new_body != body:
            changed = True
        # Reconstruct with original trailing same-line comment preserved.
        # If now empty (ignoring whitespace/commas/comments), drop the whole stanza.
        body_no_comments = re.sub(r"#[^\n]*", "", new_body)  # strip inline comments
        if re.sub(r"[ \t,\n]+", "", body_no_comments) == "":
            changed = True
            return ""  # remove the entire empty package() line
        # Otherwise, keep remaining attributes.
        return f"package({new_body}){trail}"

    new_content = PACKAGE_RE.sub(_repl, content)
    # Collapse any tall blanks that may result from removals.
    new_content = re.sub(r"\n{3,}", "\n\n", new_content)
    return new_content, changed


def process_file(p: Path) -> Tuple[str, bool, str]:
    """
    Returns (new_text, changed?, reason)
    """
    text = p.read_text(encoding="utf-8")

    # Step 1: If no package() exists, insert canonical after top-of-file loads.
    t1, inserted = insert_canonical_after_loads(text)

    # Step 2: Remove any other default_visibility from package() calls
    # (leaving a leading exact canonical stanza intact if present).
    t2, packages_changed = rewrite_packages(t1, skip_first_if_canonical=True)

    changed = inserted or packages_changed
    reason = (
        "inserted-package"
        if inserted and not packages_changed
        else (
            (
                "packages"
                if packages_changed and not inserted
                else "inserted-package+packages"
            )
            if changed
            else ""
        )
    )
    return t2, changed, reason


def main(argv: List[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "--repo_root",
        type=Path,
        default=None,
        help="Repository root. Defaults to current Git root.",
    )
    ap.add_argument(
        "--mode",
        choices=["check", "fix"],
        default="fix",
        help="Only check or also write fixes.",
    )
    ap.add_argument(
        "--allow-dirty",
        action="store_true",
        help="Allow writes even if Git working tree is dirty.",
    )
    ap.add_argument("--verbose", action="store_true", help="Verbose output.")
    args = ap.parse_args(argv)

    repo = git_root(Path.cwd()) if args.repo_root is None else args.repo_root.resolve()

    dirty = not git_clean(repo)
    if args.mode == "fix" and dirty and not args.allow_dirty:
        print(
            "ERROR: Git working tree is not clean. Aborting (use --allow-dirty to override).",
            file=sys.stderr,
        )
        return 3

    changed_any = False
    to_write: List[Tuple[Path, str]] = []

    for f in sorted(find_build_files(repo)):
        try:
            new_text, changed, reason = process_file(f)
        except Exception as e:
            print(f"ERROR processing {f}: {e}", file=sys.stderr)
            return 4

        if changed:
            changed_any = True
            to_write.append((f, new_text))
            if args.verbose or args.mode == "check":
                print(
                    f"[would {'write' if args.mode=='fix' else 'change'}] {f}  ({reason})"
                )

    if args.mode == "check":
        if changed_any:
            print("Non-compliant BUILD files found.", file=sys.stderr)
            return 1
        print("All BUILD files compliant.")
        return 0

    # mode == fix
    if not changed_any:
        print("No changes needed.")
        return 0

    # Write (we already ensured cleanliness unless --allow-dirty)
    for f, new_text in to_write:
        f.write_text(new_text, encoding="utf-8")
        if args.verbose:
            print(f"[wrote] {f}")

    print(f"Updated {len(to_write)} file(s).")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
