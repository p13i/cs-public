#!/usr/bin/env python3
# cs/devtools/filenames_in_comments.py
#!/usr/bin/env python3
import argparse
import os
import re
import sys
from pathlib import Path

CPP_EXTS = {".cc", ".cpp", ".cxx", ".hh", ".hpp", ".hxx", ".h"}
PY_EXTS = {".py"}
BUILD_NAMES = {"BUILD", "BUILD.bazel"}

EXCLUDE_DIRS = {
    ".git",
    ".hg",
    ".svn",
    "bazel-bin",
    "bazel-out",
    "bazel-testlogs",
    ".cache",
    "__pycache__",
}

PY_SHEBANG = "#!/usr/bin/env python3\n"


def comment_prefix(p: Path) -> str:
    if p.name in BUILD_NAMES or p.suffix in PY_EXTS:
        return "#"
    return "//"  # C/C++


def is_target_file(p: Path) -> bool:
    if p.name in BUILD_NAMES:
        return True
    if p.suffix in (CPP_EXTS | PY_EXTS):
        return True
    return False


def desired_path_line(p: Path, root: Path) -> str:
    rel = os.path.relpath(p, root).replace(os.sep, "/")  # normalize
    return f"{comment_prefix(p)} {rel}\n"


def has_shebang(first_line: str) -> bool:
    return first_line.startswith("#!")


def already_has_path_line(line: str, p: Path, root: Path) -> bool:
    rel = os.path.relpath(p, root).replace(os.sep, "/")
    pattern = r"^\s*(#|//)\s*" + re.escape(rel) + r"\s*$"
    return re.match(pattern, line.strip()) is not None


def ensure_python_shebang(lines: list[str]) -> tuple[list[str], bool]:
    """
    Ensure the very first line is exactly PY_SHEBANG for Python files.
    Returns (possibly modified lines, changed_flag).
    """
    if not lines:
        return [PY_SHEBANG], True
    if has_shebang(lines[0]):
        if lines[0] == PY_SHEBANG:
            return lines, False
        # Replace any existing shebang with the desired one
        return [PY_SHEBANG] + lines[1:], True
    # Insert desired shebang at top
    return [PY_SHEBANG] + lines, True


def process_file(p: Path, root: Path, write: bool) -> bool:
    try:
        with p.open("rb") as f:
            data = f.read()
    except Exception as e:
        print(f"SKIP (read error): {p} ({e})", file=sys.stderr)
        return False

    # Handle UTF-8 BOM transparently; re-emit exactly as read
    bom = b""
    text = data
    if data.startswith(b"\xef\xbb\xbf"):
        bom = b"\xef\xbb\xbf"
        text = data[len(bom) :]

    try:
        s = text.decode("utf-8")
    except UnicodeDecodeError:
        # Binary or non-UTF8; skip
        return False

    lines = s.splitlines(True)  # keep line endings
    changed = False

    # 1) For Python files, ensure shebang as the very first line.
    if p.suffix in PY_EXTS:
        lines, shebang_changed = ensure_python_shebang(lines)
        changed = changed or shebang_changed

    # 2) Ensure the --root-relative path comment is present as the first
    #    logical line (or second line if a shebang occupies line 1).
    if not lines:
        # Empty file after shebang handling (can happen if not .py and empty)
        new_lines = [desired_path_line(p, root)]
        changed = True
    else:
        insert_at = 0
        if has_shebang(lines[0]):
            insert_at = 1
            if len(lines) > 1 and already_has_path_line(lines[1], p, root):
                new_lines = lines
            else:
                new_lines = (
                    lines[:insert_at] + [desired_path_line(p, root)] + lines[insert_at:]
                )
                changed = True
        else:
            if already_has_path_line(lines[0], p, root):
                new_lines = lines
            else:
                new_lines = [desired_path_line(p, root)] + lines
                changed = True

    if changed and write:
        try:
            with p.open("wb") as f:
                f.write(bom + "".join(new_lines).encode("utf-8"))
        except Exception as e:
            print(f"SKIP (write error): {p} ({e})", file=sys.stderr)
            return False

    return changed


def walk_and_fix(root: Path, check_only: bool) -> int:
    changed = 0
    for dirpath, dirnames, filenames in os.walk(root):
        # Prune excluded dirs
        dirnames[:] = [d for d in dirnames if d not in EXCLUDE_DIRS]
        for name in filenames:
            p = Path(dirpath) / name
            if not is_target_file(p):
                continue
            if process_file(p, root, write=not check_only):
                changed += 1
    return changed


def main():
    ap = argparse.ArgumentParser(
        description="Ensure first line comment contains the --root-relative file path, "
        "and Python files start with #!/usr/bin/env python3."
    )
    ap.add_argument(
        "--root", type=Path, default=Path("."), help="Repository root to scan"
    )
    ap.add_argument(
        "--check",
        action="store_true",
        help="Only check; exit nonzero if changes are needed",
    )
    args = ap.parse_args()

    root = args.root.resolve()
    if not root.exists():
        print(f"Root not found: {root}", file=sys.stderr)
        return 2

    changed = walk_and_fix(root, check_only=args.check)
    if args.check:
        if changed > 0:
            print(f"{changed} file(s) require updates.")
            return 1
        print("All files OK.")
        return 0
    else:
        print(f"Updated {changed} file(s).")
        return 0


if __name__ == "__main__":
    sys.exit(main())
