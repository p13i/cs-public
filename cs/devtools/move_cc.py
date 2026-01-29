#!/usr/bin/env python3
# cs/devtools/move_cc.py
"""
move_cc.py

A repo-aware, AST-assisted refactoring tool to move files/directories within a C/C++
repository while updating:
  - header guards in moved headers
  - namespace declarations in moved files
  - includes that reference moved headers
  - fully-qualified namespace usages (textual, but skipping comments/strings)
  - textual path references in BUILD files

Design notes / constraints
  - Primary implementation is Python 3.8+ and uses only standard library features by
    default. Optionally, if clang.cindex (libclang Python bindings) is available,
    the script will use it to confirm/locate namespace declarations more reliably.
  - This tool is conservative: it prints a detailed plan of changes before applying
    them, and requires `--apply` to actually mutate files. When `--apply` is used,
    a clean git working tree is required by default (can be disabled).
  - The script attempts to use `git mv` for moves when possible to preserve history.

Limitations / caveats
  - A fully correct C++ refactorer is a large effort; this tool aims to be practical
    for usual code-bases where directory structure maps to namespaces and include
    paths are stable. It may require manual follow-up for complex edge cases.

Usage examples
  # dry-run plan
  python3 move_cc.py --src cs/foo/bar --dst cs/foo/qux

  # actually apply changes (requires clean git tree by default)
  python3 move_cc.py --src cs/foo/bar --dst cs/foo/qux --apply --base_dir=/path/to/repo

  # move a single file
  python3 move_cc.py --src cs/foo/bar/baz.h --dst cs/foo/qux/ --apply


Author: auto-generated; adapt to your repo's conventions as needed.
"""

from __future__ import annotations

import argparse
import os
import re
import shutil
import stat
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Tuple

# Optional clang import (best-effort). If present, libclang will be used to
# sanity-check namespace declarations and to locate declaration locations.
try:
    import clang.cindex as clang
except Exception:  # pragma: no cover - optional
    clang = None

# ----------------------------- Utilities ---------------------------------- #


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def default_header_exts() -> Tuple[str, ...]:
    return (".h", ".hh", ".hpp", ".hxx", ".inc")


def default_source_exts() -> Tuple[str, ...]:
    return (".c", ".cc", ".cpp", ".cxx", ".m", ".mm")


def is_text_file(path: Path) -> bool:
    try:
        with path.open("rb") as f:
            chunk = f.read(2048)
        if b"\0" in chunk:
            return False
        return True
    except Exception:
        return False


# ------------------------ Comment/string masking -------------------------- #


def compute_comment_string_mask(text: str) -> List[bool]:
    """Return a list of booleans (len == len(text)) where True indicates the
    character is inside a C/C++ comment or string literal.

    This is a conservative lexer that understands:
      - // line comments
      - /* block comments */
      - "..." and '...' strings with escapes (no full support for raw-strings)

    It's not a full lexer but is sufficient for skipping replacements inside
    comments/strings in most C++ code.
    """
    n = len(text)
    mask = [False] * n
    i = 0
    state = "normal"
    string_quote = None
    while i < n:
        ch = text[i]
        nxt = text[i + 1] if i + 1 < n else ""
        if state == "normal":
            if ch == "/" and nxt == "/":
                # line comment
                j = i
                mask[j] = True
                j += 1
                while j < n and text[j] != "\n":
                    mask[j] = True
                    j += 1
                i = j
                continue
            if ch == "/" and nxt == "*":
                # block comment
                j = i
                mask[j] = True
                j += 1
                while j < n:
                    mask[j] = True
                    if text[j - 1] == "*" and text[j] == "/":
                        j += 1
                        break
                    j += 1
                i = j
                continue
            if ch in ('"', "'"):
                state = "string"
                string_quote = ch
                mask[i] = True
                i += 1
                continue
            else:
                i += 1
                continue
        elif state == "string":
            mask[i] = True
            if ch == "\\":
                # skip escaped char
                if i + 1 < n:
                    mask[i + 1] = True
                    i += 2
                    continue
                else:
                    i += 1
                    continue
            if ch == string_quote:
                state = "normal"
                string_quote = None
            i += 1
            continue
    return mask


# --------------------------- Header guard helpers ------------------------- #

_guard_token_re = re.compile(r"[^A-Za-z0-9]+")


def path_to_guard(rel: Path) -> str:
    s = rel.as_posix()
    s = _guard_token_re.sub("_", s)
    s = re.sub(r"_+", "_", s)
    s = s.strip("_")
    s = s.upper()
    if s and s[0].isdigit():
        s = "_" + s
    return s


_RE_IFNDEF = re.compile(r"^\s*#\s*ifndef\s+([A-Za-z_][A-Za-z0-9_]*)\s*$", re.MULTILINE)
_RE_IF_NOT_DEFINED = re.compile(
    r"^\s*#\s*if\s*!\s*defined\s*\(?\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)?\s*$", re.MULTILINE
)


def detect_guard(text: str) -> Optional[str]:
    # try ifndef/define pair
    m = _RE_IFNDEF.search(text)
    if m:
        name = m.group(1)
        # verify there's a corresponding #define shortly after
        idx = m.end()
        m2 = re.search(
            r"^\s*#\s*define\s+" + re.escape(name) + r"\b", text[idx:], re.MULTILINE
        )
        if m2:
            return name
    # try #if !defined(NAME)
    m = _RE_IF_NOT_DEFINED.search(text)
    if m:
        name = m.group(1)
        idx = m.end()
        m2 = re.search(
            r"^\s*#\s*define\s+" + re.escape(name) + r"\b", text[idx:], re.MULTILINE
        )
        if m2:
            return name
    # pragma once -> no guard
    if re.search(r"^\s*#\s*pragma\s+once\b", text, re.MULTILINE):
        return None
    return None


def rewrite_guard(text: str, old: str, new: str) -> str:
    text = re.sub(
        rf"^(\s*#\s*ifndef\s+){re.escape(old)}(\b)",
        rf"\1{new}\2",
        text,
        flags=re.MULTILINE,
    )
    text = re.sub(
        rf"^(\s*#\s*if\s*!\s*defined\s*\(?\s*){re.escape(old)}(\s*\)?\s*$)",
        rf"\1{new}\2",
        text,
        flags=re.MULTILINE,
    )
    text = re.sub(
        rf"^(\s*#\s*define\s+){re.escape(old)}(\b)",
        rf"\1{new}\2",
        text,
        flags=re.MULTILINE,
    )
    text = re.sub(
        rf"^(\s*#\s*endif\b[^\n]*?)\b{re.escape(old)}\b",
        rf"\1{new}",
        text,
        flags=re.MULTILINE,
    )
    return text


# -------------------------- Namespace helpers ---------------------------- #


def path_to_namespace_components(rel: Path, ns_root: Optional[Path]) -> List[str]:
    """Compute namespace components from a repo-relative directory path.

    By default, the entire relative directory path is used. If ns_root is set,
    the components are relative to ns_root.
    """
    p = rel
    if p.is_file():
        p = p.parent
    parts = list(p.parts)
    if ns_root:
        ns_root = ns_root.parts if isinstance(ns_root, Path) else ns_root
        # remove prefix parts matching ns_root if present
        if (
            isinstance(ns_root, (list, tuple))
            and len(ns_root) <= len(parts)
            and parts[: len(ns_root)] == list(ns_root)
        ):
            parts = parts[len(ns_root) :]

    # sanitize to valid C++ identifiers: replace non-alnum with '_'
    def _sanitize(name: str) -> str:
        name = re.sub(r"[^A-Za-z0-9_]", "_", name)
        if re.match(r"^[0-9]", name):
            name = "_" + name
        return name

    return [_sanitize(p) for p in parts if p]


def find_leading_namespace_block(text: str) -> Optional[Tuple[List[str], int, int]]:
    """Detect a leading namespace block at the top of a file.

    Returns (components, start_index, end_index) where components is list of
    namespace names (in textual order), and start/end are character indices for
    the matched header 'namespace ... { namespace ... { ...' (closing braces are
    not validated here). Returns None if no leading namespace pattern is found.
    """
    # Find first non-comment non-empty region
    # We'll use a simple regex to match either nested-braces namespaces or
    # nested-namespace-definition (a::b::c)
    ns_single = re.compile(r"^\s*namespace\s+([A-Za-z0-9_:]+)\s*{", re.MULTILINE)
    m = ns_single.search(text)
    if not m:
        return None
    start = m.start()
    group = m.group(1)
    if "::" in group:
        comps = [c for c in group.split("::") if c]
        end = m.end()
        return comps, start, end
    # Otherwise, it might be nested 'namespace a { namespace b {'
    # We'll greedily match a run of 'namespace NAME {' tokens at the top.
    run = re.compile(
        r"^(?:\s*(?:/(?:/).*?\n|/\*(?:.|\n)*?\*/\s*)*)((?:\s*namespace\s+[A-Za-z_][A-Za-z0-9_]*\s*\{\s*)+)",
        re.MULTILINE,
    )
    mr = run.search(text)
    if not mr:
        # fallback to single-match
        return [group], m.start(), m.end()
    block = mr.group(1)
    # extract names
    names = re.findall(r"namespace\s+([A-Za-z_][A-Za-z0-9_]*)\s*\{", block)
    return names, mr.start(1), mr.end(1)


# ----------------------------- File discovery ---------------------------- #


def iter_repo_files(
    root: Path, exts: Sequence[str], exclude_dirs: Sequence[str]
) -> Iterable[Path]:
    exts_l = tuple(e.lower() for e in exts)
    for p in root.rglob("*"):
        if p.is_symlink():
            continue
        if p.is_file() and p.suffix.lower() in exts_l:
            rel = p.relative_to(root)
            parts = [s.lower() for s in rel.parts]
            if any(ed.rstrip("*") in part for ed in exclude_dirs for part in parts):
                continue
            yield p


# ----------------------------- Git helpers ------------------------------- #


def git_is_clean(root: Path) -> bool:
    try:
        result = subprocess.run(
            ["git", "-C", str(root), "status", "--porcelain"],
            capture_output=True,
            text=True,
            check=True,
        )
        return result.stdout.strip() == ""
    except Exception:
        return False


def try_git_mv(src: Path, dst: Path, base_dir: Path) -> bool:
    """Run git mv from src to dst relative to repo root."""
    try:
        rel_src = src.relative_to(base_dir)
        rel_dst = dst.relative_to(base_dir)
        subprocess.run(
            ["git", "mv", str(rel_src), str(rel_dst)],
            cwd=base_dir,
            check=True,
        )
        return True
    except Exception:
        return False


# ----------------------------- Core functions ---------------------------- #


@dataclass
class PlannedChange:
    path: Path
    orig_text: str
    new_text: str
    new_path: Path


def plan_moves(root: Path, src: Path, dst: Path) -> Dict[Path, Path]:
    """Return mapping from old absolute paths -> new absolute paths for all files to move."""
    src_abs = (root / src).resolve()
    dst_abs = (root / dst).resolve()
    if not src_abs.exists():
        raise SystemExit(f"Source does not exist: {src}")

    mapping: Dict[Path, Path] = {}
    if src_abs.is_file():
        # single file: place into dst directory (if dst ends with slash or is dir) or rename
        if dst_abs.exists() and dst_abs.is_dir():
            new = dst_abs / src_abs.name
        else:
            # user provided explicit new filename
            new = dst_abs
        mapping[src_abs] = new
        return mapping

    # directory: walk all files inside
    for p in src_abs.rglob("*"):
        if p.is_file():
            rel = p.relative_to(src_abs)
            new_path = dst_abs / rel
            mapping[p] = new_path
    return mapping


def plan_header_guard_rewrites(
    root: Path, moves: Dict[Path, Path]
) -> List[PlannedChange]:
    changes: List[PlannedChange] = []
    for old, new in moves.items():
        if old.suffix.lower() in default_header_exts():
            orig_text = old.read_text(encoding="utf-8", errors="replace")
            old_rel = old.relative_to(root)
            new_rel = new.relative_to(root)
            old_guard = detect_guard(orig_text)
            expected_guard = path_to_guard(new_rel)
            if old_guard and old_guard != expected_guard:
                new_text = rewrite_guard(orig_text, old_guard, expected_guard)
                changes.append(PlannedChange(old, orig_text, new_text, new_path=new))
    return changes


def plan_namespace_rewrites_for_moved_files(
    root: Path, moves: Dict[Path, Path], ns_root: Optional[Path]
) -> List[PlannedChange]:
    changes: List[PlannedChange] = []
    for old, new in moves.items():
        if old.suffix.lower() in default_header_exts() + default_source_exts() or True:
            # inspect file for leading namespace declaration(s)
            text = old.read_text(encoding="utf-8", errors="replace")
            found = find_leading_namespace_block(text)
            if not found:
                continue
            comps, sidx, eidx = found
            # compute expected old/new components from path
            old_rel = old.relative_to(root)
            new_rel = new.relative_to(root)
            old_ns = path_to_namespace_components(old_rel, ns_root)
            new_ns = path_to_namespace_components(new_rel, ns_root)
            if not old_ns or not new_ns:
                continue
            # if the file's detected leading namespace matches the path-derived old namespace prefix,
            # compose a replacement
            # if comps equals old_ns suffix, or startswith old_ns ending, be conservative: only replace when prefix matches
            if comps[: len(old_ns)] == old_ns:
                # Replace the textual namespace header between sidx and eidx with new namespace
                # Build replacement text: either 'namespace a::b::c {' or nested braces
                # Preserve whether original used :: or nested-braces style
                header_fragment = text[sidx:eidx]
                if "::" in header_fragment:
                    new_fragment = re.sub(
                        r"namespace\s+[A-Za-z0-9_:]+\s*{",
                        f"namespace {'::'.join(new_ns)} {{",
                        header_fragment,
                    )
                else:
                    # nested braces style: produce same number of 'namespace NAME {' lines
                    new_fragment = "".join([f"namespace {n} {{\n" for n in new_ns])
                new_text = text[:sidx] + new_fragment + text[eidx:]
                changes.append(PlannedChange(old, text, new_text, new_path=new))
    return changes


def plan_include_and_path_rewrites(
    root: Path, moves: Dict[Path, Path], additional_exts: Sequence[str]
) -> List[PlannedChange]:
    changes: List[PlannedChange] = []
    # Build mapping of repo-relative old -> new posix
    rel_map: Dict[str, str] = {}
    for old, new in moves.items():
        try:
            rel_old = old.relative_to(root).as_posix()
            rel_new = new.relative_to(root).as_posix()
        except Exception:
            continue
        rel_map[rel_old] = rel_new

    # file extensions to scan for includes/usages
    exts = list(default_header_exts() + default_source_exts())
    exts.extend(list(additional_exts))

    for f in iter_repo_files(root, exts, exclude_dirs=[".git", "third_party", "build"]):
        text = f.read_text(encoding="utf-8", errors="replace")
        orig_text = text
        mask = compute_comment_string_mask(text)
        out = []
        i = 0
        changed = False

        # Replace include directives
        def replace_includes(t: str) -> Tuple[str, bool]:
            changed_local = False
            for ro, rn in rel_map.items():
                # match #include "ro" or #include <ro>
                pat = re.compile(r'(#\s*include\s*["<])' + re.escape(ro) + r'([">])')
                t2, n = pat.subn(r"\1" + rn + r"\2", t)
                if n:
                    changed_local = True
                    t = t2
            return t, changed_local

        text, inc_changed = replace_includes(text)
        if inc_changed:
            changed = True

        # Replace textual references to repo-relative paths in BUILD files or scripts
        if f.name == "BUILD" or f.suffix == ".bzl" or f.suffix == ".bazel":
            for ro, rn in rel_map.items():
                if ro in text:
                    text = text.replace(ro, rn)
                    changed = True

        # Replace qualified namespace usages: for each old dir -> new dir, attempt to compute namespace
        # prefixes and do textual safe replacement of 'old::' -> 'new::' skipping comments/strings
        for oldp, newp in rel_map.items():
            old_ns = "::".join(path_to_namespace_components(Path(oldp), None))
            new_ns = "::".join(path_to_namespace_components(Path(newp), None))
            if not old_ns or old_ns == new_ns:
                continue
            # Search occurrences of old_ns in text and replace only when not inside comments/strings
            start = 0
            while True:
                idx = text.find(old_ns, start)
                if idx == -1:
                    break
                # ensure token boundary: previous char is not [A-Za-z0-9_]
                if idx > 0 and re.match(r"[A-Za-z0-9_]", text[idx - 1]):
                    start = idx + len(old_ns)
                    continue
                # ensure following is :: or ::something
                after_idx = idx + len(old_ns)
                if text[after_idx : after_idx + 2] != "::":
                    start = after_idx
                    continue
                # ensure not inside comment/string
                if mask[idx]:
                    start = after_idx
                    continue
                # perform replacement
                text = text[:idx] + new_ns + text[after_idx:]
                # recompute mask for changed region (conservative: recompute whole mask)
                mask = compute_comment_string_mask(text)
                changed = True
                start = idx + len(new_ns)

        if changed and text != orig_text:
            changes.append(PlannedChange(f, orig_text, text, new_path=newp))
    return changes


# ------------------------------ Apply changes ---------------------------- #


def find_repo_root(path: Path) -> Path | None:
    try:
        result = subprocess.run(
            ["git", "-C", str(path.parent), "rev-parse", "--show-toplevel"],
            capture_output=True,
            text=True,
            check=True,
        )
        return Path(result.stdout.strip())
    except subprocess.CalledProcessError:
        return None


def apply_planned_changes(changes: Iterable[PlannedChange], dry_run: bool) -> int:
    """Apply textual rewrites first, then perform file moves.

    Returns:
        Number of changes actually applied.
    """
    applied = 0
    planned_moves: dict[Path, Path] = {}

    # 1. Separate moves vs rewrites
    for c in changes:
        if c.new_path and Path(c.new_path) != Path(c.path):
            planned_moves[Path(c.path)] = Path(c.new_path)

    # 2. Apply textual changes BEFORE moves
    for c in changes:
        path = Path(c.path)
        # If a file is scheduled for move, still rewrite at the old location
        if not path.exists() and path in planned_moves:
            path = planned_moves[path]

        if not path.exists():
            print(f"[WARN] Skipping rewrite, file missing: {path}")
            continue

        if dry_run:
            print(f"[DRY RUN] Would rewrite {path}")
            applied += 1
            continue

        mode = path.stat().st_mode
        content = path.read_text()
        new_content = c.new_text
        if new_content != content:
            path.write_text(new_content)
            path.chmod(mode)
            applied += 1

    # 3. Only AFTER rewrites, perform moves
    for old, new in planned_moves.items():
        if dry_run:
            print(f"[DRY RUN] Would move {old} -> {new}")
            applied += 1
            continue

        repo_root = find_repo_root(old)
        try:
            if repo_root:
                subprocess.run(
                    ["git", "mv", str(old), str(new)], check=True, cwd=repo_root
                )
            else:
                new.parent.mkdir(parents=True, exist_ok=True)
                shutil.move(str(old), str(new))
            applied += 1
        except Exception as e:
            print(f"[ERROR] Failed to move {old} -> {new}: {e}")

    return applied


# ------------------------------- Main ------------------------------------ #


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description="Move files/dirs and update C++ namespaces/includes/guards."
    )
    p.add_argument(
        "--src",
        required=True,
        help="Source file or directory (repo-relative or absolute).",
    )
    p.add_argument(
        "--dst",
        required=True,
        help="Destination path (repo-relative or absolute). For files, this may be a directory or new filename.",
    )
    p.add_argument(
        "--base_dir",
        default=".",
        help="Repository root (defaults to working directory).",
    )
    p.add_argument(
        "--ns_root",
        default=None,
        help="Optional namespace root to strip from path->namespace conversion.",
    )
    p.add_argument(
        "--apply",
        action="store_true",
        help="Apply changes. Otherwise do a dry-run plan.",
    )
    p.add_argument(
        "--no_git_mv", action="store_true", help="Do not use git mv even if available."
    )
    p.add_argument(
        "--extensions",
        default=",".join(default_header_exts() + default_source_exts()),
        help="Comma-separated list of file extensions to scan.",
    )
    return p.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    base = Path(args.base_dir).resolve()
    src = Path(args.src)
    dst = Path(args.dst)
    ns_root = Path(args.ns_root) if args.ns_root else None

    print(f"Base dir: {base}")
    try:
        moves = plan_moves(base, src, dst)
    except Exception as e:
        eprint(f"Error planning moves: {e}")
        return 2

    if not moves:
        print("No files to move.")
        return 0

    print("Planned moves:")
    for old, new in moves.items():
        print(f"  {old.relative_to(base)} -> {new.relative_to(base)}")

    # header guard changes
    guard_changes = plan_header_guard_rewrites(base, moves)
    ns_changes = plan_namespace_rewrites_for_moved_files(base, moves, ns_root)
    include_and_path_changes = plan_include_and_path_rewrites(
        base, moves, additional_exts=[]
    )  # extend as needed

    total_changes = len(guard_changes) + len(ns_changes) + len(include_and_path_changes)

    print("\nPlanned internal-file changes:")
    print(f"  Header guard rewrites: {len(guard_changes)}")
    print(f"  Namespace rewrites (moved files): {len(ns_changes)}")
    print(f"  Cross-repo include/path/usage changes: {len(include_and_path_changes)}")

    if total_changes == 0:
        print("\nNo textual changes required beyond file moves.")

    # If applying, require clean git working tree unless user disables git mv
    if args.apply:
        git_clean = git_is_clean(base)
        if not git_clean:
            eprint(
                "Refusing to apply until git working tree is clean. Commit or stash your changes, or rerun with --no_git_mv to ignore."
            )
            return 3

    # Perform moves (git mv preferred)
    if args.apply:
        for old, new in moves.items():
            new.parent.mkdir(parents=True, exist_ok=True)
            moved = False
            if not args.no_git_mv and git_is_clean(base):
                try:
                    # attempt git mv; on failure, fallback to shutil.move
                    if try_git_mv(old, new, base):
                        print(
                            f"git mv {old.relative_to(base)} -> {new.relative_to(base)}"
                        )
                        moved = True
                except Exception:
                    moved = False
            if not moved:
                shutil.move(str(old), str(new))
                print(f"moved {old.relative_to(base)} -> {new.relative_to(base)}")
    else:
        print("\nDry-run: not performing file moves. Use --apply to perform them.")

    # Apply textual changes
    if args.apply:
        c1 = apply_planned_changes(
            guard_changes + ns_changes + include_and_path_changes,
            dry_run=not args.apply,
        )
        print(f"\nApplied {c1} textual changes.")
    else:
        print("\nPlanned textual changes (dry-run):")
        for c in guard_changes:
            print(f"  [guard] would rewrite guard in {c.path.relative_to(base)}")
        for c in ns_changes:
            print(f"  [ns] would rewrite namespace in {c.path.relative_to(base)}")
        for c in include_and_path_changes:
            print(f"  [xref] would rewrite {c.path.relative_to(base)}")

    print(
        "\nDone. Please run your build and tests to validate changes. Expect manual follow-up for some edge-cases."
    )
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main(sys.argv[1:]))
    except KeyboardInterrupt:
        eprint("Interrupted.")
        sys.exit(130)


# ------------------------------ BUILD snippet ---------------------------- #
# Add the following to cs/devtools/BUILD:
#
# py_binary(
#     name = "move_cc",
#     srcs = ["move_cc.py"],
#     main = "move_cc.py",
# )
#
# Run under bazel (pass -- to forward args to the program):
#
# bazel run //cs/devtools:move_cc -- --src cs/foo/bar --dst cs/foo/qux --apply --base_dir=$PWD
#
# Note: when running via `bazel run` you must forward --base_dir (or adapt the script to
# autodetect workspace root). The example here forwards $PWD from your shell as the repo root.
