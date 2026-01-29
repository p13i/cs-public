#!/usr/bin/env python3
# cs/devtools/update_env.py
"""
Update a .env file with VERSION and COMMIT from Git.

Changes vs previous:
- All KEY=VALUE pairs are sorted alphabetically by KEY.
- Output ends with exactly one newline.
- Non-KV lines (comments/blank) are preserved and placed at the top, followed by a blank line, then sorted KVs.

Usage:
  bazel run //tools:update_env -- --env /absolute/or/relative/path/.env
"""

from __future__ import annotations
import argparse
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Dict, Tuple, List

KV_RE = re.compile(r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(.*)\s*$")


def run_git(args: List[str], cwd: Path | None = None) -> str:
    try:
        out = subprocess.check_output(["git", *args], cwd=str(cwd) if cwd else None)
        return out.decode("utf-8", "replace").strip()
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"git {' '.join(args)} failed: {e}") from e
    except FileNotFoundError as e:
        raise RuntimeError("git executable not found in PATH") from e


def repo_root() -> Path:
    return Path(run_git(["rev-parse", "--show-toplevel"]))


def latest_version_tag(match: str, fallback: str) -> str:
    try:
        listing = run_git(["tag", "--list", match, "--sort=-v:refname"])
        tag = listing.splitlines()[0].strip() if listing.strip() else ""
        return tag or fallback
    except Exception:
        return fallback


def short_commit(length: int) -> str:
    return run_git(["rev-parse", f"--short={length}", "HEAD"])


def parse_env(text: str) -> Tuple[Dict[str, str], List[str]]:
    """
    Parse .env into (kv_map, non_kv_lines).
    - kv_map: last value wins for duplicate keys; values are kept exactly as text after '='
    - non_kv_lines: original non-KV lines (comments/blank/other), order preserved
    """
    kv: Dict[str, str] = {}
    non_kv: List[str] = []
    for raw in text.splitlines():
        m = KV_RE.match(raw)
        if m:
            key, value = m.group(1), m.group(2)
            kv[key] = value
        else:
            non_kv.append(raw)
    return kv, non_kv


def render_env(kv: Dict[str, str], non_kv: List[str]) -> str:
    """
    Render with:
      [non_kv...]
      (blank line iff both sections exist)
      sorted KEY=VALUE lines
    Ensure exactly one trailing newline.
    """
    lines: List[str] = []
    # Preserve non-KV lines at top (if any)
    if non_kv:
        lines.extend(non_kv)
    # Sorted KVs
    if kv:
        if non_kv:
            lines.append("")  # single separator line between comments and KVs
        for k in sorted(kv.keys()):
            lines.append(f"{k}={kv[k]}")
    # Exactly one trailing newline
    return ("\n".join(lines)).rstrip("\n") + "\n"


def atomic_write(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(
        "w", delete=False, encoding="utf-8", dir=str(path.parent)
    ) as tmp:
        tmp.write(content)
        tmp_path = Path(tmp.name)
    if path.exists():
        try:
            os.chmod(tmp_path, os.stat(path).st_mode)
        except Exception:
            pass
    os.replace(tmp_path, path)


def main() -> int:
    ap = argparse.ArgumentParser(
        description="Update .env with VERSION and COMMIT from Git (sorted output, single trailing newline)."
    )
    ap.add_argument(
        "--env", default=".env", help="Path to .env (default: .env at repo root)"
    )
    ap.add_argument(
        "--match", default="v*", help="Tag match pattern for releases (default: v*)"
    )
    ap.add_argument(
        "--fallback-version",
        default="v0.0.1",
        help="Fallback VERSION when no tag found",
    )
    ap.add_argument(
        "--commit-length", type=int, default=7, help="Short commit length (default: 7)"
    )
    ap.add_argument(
        "--cwd", default=None, help="Run git in this directory (defaults to repo root)"
    )
    args = ap.parse_args()

    try:
        root = Path(args.cwd) if args.cwd else repo_root()
    except Exception as e:
        print(f"ERROR: cannot locate repo root: {e}", file=sys.stderr)
        return 2

    env_path = (root / args.env).resolve()

    try:
        version = latest_version_tag(args.match, args.fallback_version)
        commit = short_commit(args.commit_length)
    except Exception as e:
        print(f"ERROR: failed to derive Git info: {e}", file=sys.stderr)
        return 3

    existing = ""
    if env_path.exists():
        existing = env_path.read_text(encoding="utf-8", errors="replace")

    kv, non_kv = parse_env(existing)

    # Apply our updates; last value wins
    kv["VERSION"] = version
    kv["COMMIT"] = commit

    out = render_env(kv, non_kv)
    atomic_write(env_path, out)
    print(f"Updated {env_path}: VERSION={version}, COMMIT={commit}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
