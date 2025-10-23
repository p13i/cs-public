#!/usr/bin/env python3
"""
Create an `acl.json` file in every directory under a root path where one
doesn't already exist.

Usage:
  python3 scripts/add_acls.py [--root PATH] [--dry-run] [--verbose]

Defaults to the current directory as root.
"""
import argparse
import json
import os
import sys
from typing import Set

DEFAULT_ACL = {
    "policies": [
        {
            "principals": ["*"],
            "resources": ["*"],
            "allow": False,
        }
    ]
}

SKIP_DIR_NAMES: Set[str] = {
    "bazel-bin",
    "bazel-out",
    "bazel-testlogs",
    ".git",
    "node_modules",
    "__pycache__",
    "venv",
    "env",
}


def parse_args():
    p = argparse.ArgumentParser(description="Create acl.json files where missing")
    p.add_argument("--root", "-r", default=".", help="Root directory to walk")
    p.add_argument(
        "--dry-run",
        action="store_true",
        help="Print what would be created but don't write files",
    )
    p.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    return p.parse_args()


def should_skip_dirname(name: str) -> bool:
    return name in SKIP_DIR_NAMES


def main() -> int:
    args = parse_args()
    root = os.path.abspath(args.root)

    if not os.path.isdir(root):
        print(f"Root path is not a directory: {root}")
        return 2

    created = 0
    skipped_existing = 0
    errors = 0

    for dirpath, dirnames, filenames in os.walk(root):
        # mutate dirnames in-place to avoid walking into skip dirs
        dirnames[:] = [d for d in dirnames if not should_skip_dirname(d)]

        # Skip directories that are themselves in the skip set by name
        base = os.path.basename(dirpath)
        if should_skip_dirname(base):
            if args.verbose:
                print(f"Skipping directory by name: {dirpath}")
            continue

        target = os.path.join(dirpath, "acl.json")
        if "acl.json" in filenames:
            skipped_existing += 1
            if args.verbose:
                print(f"Exists: {target}")
            continue

        if args.dry_run:
            print(f"Would create: {target}")
            created += 1
            continue

        # Write atomically to avoid partial files
        temp_path = target + ".tmp"
        try:
            with open(temp_path, "w", encoding="utf-8") as f:
                json.dump(DEFAULT_ACL, f, indent=2, sort_keys=False)
                f.write("\n")
            os.replace(temp_path, target)
            created += 1
            if args.verbose:
                print(f"Created: {target}")
        except Exception as e:
            errors += 1
            print(f"Error writing {target}: {e}", file=sys.stderr)
            try:
                if os.path.exists(temp_path):
                    os.remove(temp_path)
            except Exception:
                pass

    print("--- summary ---")
    print(f"root: {root}")
    print(f"created: {created}")
    print(f"skipped (existing): {skipped_existing}")
    print(f"errors: {errors}")

    return 0 if errors == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
