#!/usr/bin/env python3
# cs/devtools/format_black.py
# Bazel-run wrapper for Black (uses the Black library via @pip).
import sys
from typing import List


def main(argv: List[str]) -> int:
    # Defer to Black's CLI (respects pyproject.toml if present).
    try:
        from black.__main__ import main as black_main
    except Exception as e:
        print(f"[format_black] failed to import black: {e}", file=sys.stderr)
        return 1
    # If no args, format current directory.
    args = argv if argv else ["."]
    try:
        return black_main(args)
    except SystemExit as se:
        # black_main may sys.exit(); convert to int.
        return int(se.code) if se.code is not None else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
