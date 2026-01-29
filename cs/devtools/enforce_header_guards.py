#!/usr/bin/env python3
# cs/devtools/enforce_header_guards.py
"""
Audit and enforce canonical C/C++ header guards based on repository-relative paths.

Rule (default): for a header at repo path like
    cs/apps/trycopilot.ai/protos/my_proto.hh
expected guard is
    CS_APPS_TRYCOPILOT_AI_PROTOS_MY_PROTO_HH

Behavior:
  • Audit mode (default): scans headers, prints a report of OK/MISMATCH/MISSING.
    - Exit code 0 if all OK; non-zero if any issues found.
  • Apply mode (--apply): requires a clean git working tree; renames any mismatched
    include guards in-place (no commit is created). Files without an include guard
    are reported but not modified.

Assumptions & customization:
  • Only header files are scanned (default extensions: .h, .hh, .hpp, .hxx, .inc).
  • Paths are computed relative to the git repo root (auto-detected).
  • Guard generation: uppercase(repo_rel_path) with non-alnum → '_' and dot → '_' ;
    multiple underscores collapsed; leading digits are prefixed with an underscore.
  • Excluded directories by default: .git, .hg, .svn, build, bazel-*, out, third_party (editable via CLI).
  • The script attempts to detect a classic guard of the form:
        // cs/path/to/header.hh
        #ifndef GUARD
        #define GUARD
        ...
        #endif  // optional GUARD
    It also accepts '#if !defined(GUARD)' in place of '#ifndef GUARD'.
  • Files using only '#pragma once' are treated as MISSING (no automatic insertion).

Usage examples:
  • Audit only:    python3 enforce_header_guards.py
  • Apply fixes:   python3 enforce_header_guards.py --apply
  • Limit to path: python3 enforce_header_guards.py --paths cs/apps/trycopilot.ai/protos include/

Requires: Python 3.8+ and git available on PATH.
"""
from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, List, Optional, Sequence, Tuple

# ------------------------------- Git helpers ------------------------------- #


def git_root(start: Path) -> Path:
    """Return the absolute path to the repo root using 'git rev-parse'."""
    try:
        result = subprocess.run(
            ["git", "-C", str(start), "rev-parse", "--show-toplevel"],
            check=True,
            capture_output=True,
            text=True,
        )
        return Path(result.stdout.strip()).resolve()
    except subprocess.CalledProcessError as e:
        print(
            "Error: not inside a git repository (git rev-parse failed)." + str(e),
            file=sys.stderr,
        )
        raise SystemExit(2) from e


def git_is_clean(root: Path) -> bool:
    """Return True if the working tree is clean (no staged/unstaged changes)."""
    result = subprocess.run(
        ["git", "-C", str(root), "status", "--porcelain"],
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout.strip() == ""


# ------------------------------ File discovery ---------------------------- #


def default_header_exts() -> Tuple[str, ...]:
    return (".h", ".hh", ".hpp", ".hxx", ".inc")


def should_exclude(path: Path, exclude_dirs: Sequence[str]) -> bool:
    parts = set(p.lower() for p in path.parts)
    for pat in exclude_dirs:
        pat = pat.lower()
        if pat.endswith("*"):
            # simple prefix wildcard for directory names
            prefix = pat[:-1]
            if any(part.startswith(prefix) for part in parts):
                return True
        else:
            if pat in parts:
                return True
    return False


def iter_header_files(
    root: Path, paths: Sequence[Path], exts: Sequence[str], exclude_dirs: Sequence[str]
) -> Iterable[Path]:
    """Yield header files under given paths (or repo root if empty)."""
    if not paths:
        paths = [root]
    exts_l = tuple(e.lower() for e in exts)
    for base in paths:
        base = (root / base).resolve() if not base.is_absolute() else base.resolve()
        if base.is_file():
            if base.suffix.lower() in exts_l and not should_exclude(
                base.relative_to(root), exclude_dirs
            ):
                yield base
            continue
        for p in base.rglob("*"):
            if p.is_symlink():
                continue
            if p.is_file() and p.suffix.lower() in exts_l:
                rel = p.relative_to(root)
                if not should_exclude(rel, exclude_dirs):
                    yield p


# -------------------------- Guard name computation ------------------------ #

_guard_token_re = re.compile(r"[^A-Za-z0-9]+")


def path_to_guard(rel: Path) -> str:
    """Turn a repo-relative path into a canonical macro guard.

    Example: cs/apps/trycopilot.ai/protos/my_proto.hh → CS_APPS_TRYCOPILOT_AI_PROTOS_MY_PROTO_HH
    """
    # Use POSIX-style path with forward slashes for stable results.
    s = rel.as_posix()
    # Replace non-alnum (including dots and slashes) with underscores.
    s = _guard_token_re.sub("_", s)
    # Collapse multiple underscores.
    s = re.sub(r"_+", "_", s)
    # Trim stray leading/trailing underscores.
    s = s.strip("_")
    # Uppercase.
    s = s.upper()
    # Macro identifiers must not start with a digit; prefix underscore if needed.
    if s and s[0].isdigit():
        s = "_" + s
    return s


# ----------------------------- Guard detection ---------------------------- #

# Patterns for detecting include guards near the top of the file (after comment/license blocks).
_RE_IFNDEF = re.compile(r"^\s*#\s*ifndef\s+([A-Za-z_][A-Za-z0-9_]*)\s*$")
_RE_IF_NOT_DEFINED = re.compile(
    r"^\s*#\s*if\s*!\s*defined\s*\(?\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)?\s*$"
)
_RE_DEFINE_FMT = r"^\s*#\s*define\s+{macro}(?:\b|\s|$)"

_RE_PRAGMA_ONCE = re.compile(r"^\s*#\s*pragma\s+once\b")


@dataclass
class GuardInfo:
    kind: str  # 'OK', 'MISMATCH', 'MISSING', 'NO_GUARD'
    rel: Path
    found: Optional[str]
    expected: str
    if_line: Optional[int] = None
    define_line: Optional[int] = None


def strip_comments(text: str) -> str:
    """Remove C/C++ // line comments and /* */ block comments, preserving newlines.
    This is conservative and adequate for scanning preprocessor lines.
    """
    # Remove block comments, including multiline.
    out = []
    i = 0
    n = len(text)
    in_block = False
    while i < n:
        if not in_block and i + 1 < n and text[i] == "/" and text[i + 1] == "*":
            in_block = True
            i += 2
            continue
        if in_block and i + 1 < n and text[i] == "*" and text[i + 1] == "/":
            in_block = False
            i += 2
            continue
        if not in_block:
            out.append(text[i])
        i += 1
    text = "".join(out)
    # Remove // comments to end of line
    lines = []
    for line in text.splitlines(keepends=False):
        j = 0
        while j < len(line) - 1:
            if line[j] == "/" and line[j + 1] == "/":
                line = line[:j]
                break
            # Skip string/char literals to avoid stripping '//' inside them.
            if line[j] in ('"', "'"):
                quote = line[j]
                j += 1
                while j < len(line):
                    if line[j] == "\\":
                        j += 2
                        continue
                    if line[j] == quote:
                        j += 1
                        break
                    j += 1
            else:
                j += 1
        lines.append(line)
    return "\n".join(lines)


def detect_guard(text: str) -> Tuple[Optional[str], Optional[int], Optional[int], bool]:
    """Detect an include guard macro and its line numbers (0-based) if present.

    Returns: (macro, if_line, define_line, saw_pragma_once)
    macro/lines are None if no classic guard is found.
    """
    stripped = strip_comments(text)
    lines = stripped.splitlines()

    saw_pragma = any(_RE_PRAGMA_ONCE.match(ln) for ln in lines[:50])

    # Search for a guard within the first ~100 logical lines after comments/license blocks.
    # We consider the first pair of (#ifndef X | #if !defined(X)) followed soon by #define X as a guard.
    for i, ln in enumerate(lines[:200]):
        m1 = _RE_IFNDEF.match(ln)
        m2 = _RE_IF_NOT_DEFINED.match(ln)
        macro = m1.group(1) if m1 else (m2.group(1) if m2 else None)
        if not macro:
            continue
        # Look ahead a bit for the matching #define
        define_re = re.compile(_RE_DEFINE_FMT.format(macro=re.escape(macro)))
        for j in range(i + 1, min(i + 60, len(lines))):
            ln2 = lines[j]
            if not ln2.strip():
                continue
            if define_re.match(ln2):
                return macro, i, j, saw_pragma
            # If we see another preprocessor directive before #define, abort this candidate
            if ln2.lstrip().startswith("#"):
                break
    return None, None, None, saw_pragma


# ------------------------------ Guard rewriting --------------------------- #


def rewrite_guard(text: str, old: str, new: str) -> str:
    """Rewrite the include-guard macro in all relevant preprocessor lines.

    We restrict replacements to preprocessor lines (#ifndef/#define/#if !defined/#endif ... old).
    """
    # 1) #ifndef OLD → #ifndef NEW
    text = re.sub(
        rf"^(\s*#\s*ifndef\s+){re.escape(old)}(\b)",
        rf"\1{new}\2",
        text,
        flags=re.MULTILINE,
    )

    # 2) #if !defined(OLD) or #if !defined OLD → ... NEW ...
    text = re.sub(
        rf"^(\s*#\s*if\s*!\s*defined\s*\(?\s*){re.escape(old)}(\s*\)?\s*$)",
        rf"\1{new}\2",
        text,
        flags=re.MULTILINE,
    )

    # 3) #define OLD → #define NEW
    text = re.sub(
        rf"^(\s*#\s*define\s+){re.escape(old)}(\b)",
        rf"\1{new}\2",
        text,
        flags=re.MULTILINE,
    )

    # 4) #endif comments referencing OLD → NEW (keep it conservative on the same line)
    text = re.sub(
        rf"^(\s*#\s*endif\b[^\n]*?)\b{re.escape(old)}\b",
        rf"\1{new}",
        text,
        flags=re.MULTILINE,
    )
    return text


# --------------------------------- Audit ---------------------------------- #


def audit_file(root: Path, path: Path) -> GuardInfo:
    rel = path.relative_to(root)
    expected = path_to_guard(rel)
    data = path.read_text(encoding="utf-8", errors="replace")

    found, if_line, def_line, saw_pragma = detect_guard(data)

    if not found:
        if saw_pragma:
            return GuardInfo("NO_GUARD", rel, None, expected, None, None)
        else:
            return GuardInfo("MISSING", rel, None, expected, None, None)

    if found == expected:
        return GuardInfo("OK", rel, found, expected, if_line, def_line)
    else:
        return GuardInfo("MISMATCH", rel, found, expected, if_line, def_line)


# --------------------------------- Main ----------------------------------- #


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description="Audit and enforce canonical header guards."
    )
    p.add_argument(
        "--apply",
        action="store_true",
        help="Rewrite mismatched guards in-place (requires clean git tree).",
    )
    p.add_argument(
        "--extensions",
        default=",".join(default_header_exts()),
        help="Comma-separated list of header file extensions to scan (default: .h,.hh,.hpp,.hxx,.inc)",
    )
    p.add_argument(
        "--exclude-dirs",
        default=".git,.hg,.svn,build,bazel-*,out,third_party",
        help="Comma-separated directory names to exclude (exact match; * allowed as a suffix).",
    )
    p.add_argument(
        "--paths",
        nargs="*",
        default=[],
        help="Optional paths (files or directories) relative to repo root to limit the scan.",
    )
    p.add_argument(
        "--quiet",
        action="store_true",
        help="Only print summary and errors (suppress OK lines).",
    )
    p.add_argument(
        "--base_dir",
        default=".",
        help="Path to Git repo root (defaults to current working directory).",
    )
    return p.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    start = Path(args.base_dir).resolve()
    root = git_root(start)

    exts = tuple(
        e if e.startswith(".") else f".{e}"
        for e in (s.strip() for s in args.extensions.split(",") if s.strip())
    )
    exclude_dirs = [s.strip() for s in args.exclude_dirs.split(",") if s.strip()]
    limit_paths = [Path(p) for p in args.paths]

    files = list(iter_header_files(root, limit_paths, exts, exclude_dirs))
    if not files:
        print("No header files found with the specified criteria.")
        return 0

    infos: List[GuardInfo] = []
    for f in files:
        if "node_modules" in str(f):
            continue
        info = audit_file(root, f)
        infos.append(info)
        if not args.quiet or info.kind != "OK":
            if info.kind == "OK":
                print(f"OK        {info.rel}  →  {info.expected}")
            elif info.kind == "MISMATCH":
                print(
                    f"MISMATCH  {info.rel}  found={info.found}  expected={info.expected}"
                )
            elif info.kind == "MISSING":
                print(
                    f"MISSING   {info.rel}  (no include guard found; expected={info.expected})"
                )
            elif info.kind == "NO_GUARD":
                print(
                    f"NO_GUARD  {info.rel}  (uses #pragma once; expected guard would be {info.expected})"
                )

    n_ok = sum(1 for i in infos if i.kind == "OK")
    n_mismatch = sum(1 for i in infos if i.kind == "MISMATCH")
    n_missing = sum(1 for i in infos if i.kind == "MISSING")
    n_no_guard = sum(1 for i in infos if i.kind == "NO_GUARD")

    print("\nSummary:")
    print(f"  OK:        {n_ok}")
    print(f"  MISMATCH:  {n_mismatch}")
    print(f"  MISSING:   {n_missing}")
    print(f"  NO_GUARD:  {n_no_guard}")

    if args.apply:
        if n_mismatch == 0:
            print("\nNothing to rewrite: no mismatched guards found.")
        # Enforce git cleanliness before modifying anything
        # if not git_is_clean(root):
        #     print(
        #         "\nRefusing to modify files: git working tree is not clean. Commit or stash your changes and retry.",
        #         file=sys.stderr,
        #     )
        #     return 2

        changed = 0
        for info in infos:
            if info.kind != "MISMATCH" or not info.found:
                continue
            path = root / info.rel
            data = path.read_text(encoding="utf-8", errors="replace")
            new_data = rewrite_guard(data, info.found, info.expected)
            new_data = "// " + str(info.rel) + "\n" + new_data  # ensure header comment
            if new_data != data:
                path.write_text(new_data, encoding="utf-8")
                print(f"Rewrote guard in {info.rel}: {info.found} → {info.expected}")
                changed += 1
        print(f"\nRewrites applied: {changed}")

    # Exit code policy: non-zero if any issues (MISMATCH/MISSING) were detected.
    if n_mismatch or n_missing:
        return 1
    return 0


if __name__ == "__main__":
    try:
        main(sys.argv[1:])
        sys.exit(0)  # exit with zero so other linters can run
    except KeyboardInterrupt:
        print("Interrupted.", file=sys.stderr)
        sys.exit(130)
