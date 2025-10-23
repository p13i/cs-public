#!/usr/bin/env python3
"""
Run a GitHub Actions workflow *locally* using `act`, with robust image pre-pull,
retries, and optional auto-install of act into a user-writable directory.

Minimal required args:
  --workflow-yml  Path to the workflow file *relative* to --repo-root
  --repo-root     Filesystem root of the repo to run in (e.g., $CURDIR)

Optional args let you pick event, job, runner image(s), retries and timeouts.

Example:
  ./gha.py \
    --workflow-yml .github/workflows/bazel.yml \
    --repo-root "$(pwd)" \
    --event push \
    --job build
"""

import argparse
import os
import shlex
import shutil
import subprocess
import sys
import time
from pathlib import Path


def run(cmd, *, cwd=None, timeout=None, check=False, env=None, capture_output=False):
    return subprocess.run(
        cmd,
        cwd=cwd,
        timeout=timeout,
        check=check,
        env=env,
        text=True,
        capture_output=capture_output,
    )


def which(prog):
    p = shutil.which(prog)
    return Path(p) if p else None


def ensure_docker_running():
    try:
        run(["docker", "info"], timeout=10, check=True, capture_output=True)
    except Exception as e:
        print(
            "ERROR: Docker is required and must be running (docker info failed).",
            file=sys.stderr,
        )
        raise e


def find_or_install_act(
    repo_root: Path, preferred_dir: Path | None, version: str | None
) -> Path:
    # 1) Already on PATH?
    act = which("act")
    if act:
        return act

    # 2) Common user-writable locations (first writable wins)
    candidates = []
    if preferred_dir:
        candidates.append(preferred_dir)
    candidates.extend(
        [
            Path.home() / ".local" / "bin",
            Path.home() / "bin",
            repo_root / ".tools" / "bin",
        ]
    )
    install_dir = None
    for d in candidates:
        try:
            d.mkdir(parents=True, exist_ok=True)
            if os.access(d, os.W_OK):
                install_dir = d
                break
        except Exception:
            continue
    if not install_dir:
        raise RuntimeError(
            "No writable install dir found for act (tried ~/.local/bin, ~/bin, .tools/bin)."
        )

    # 3) Install via upstream script (keeps this script dependency-free)
    print(f"Installing act into {install_dir} ...")
    ver_flag = []
    if version:
        ver_flag = ["-v", version]
    # Using bash install script
    cmd = f'curl -sSL https://raw.githubusercontent.com/nektos/act/master/install.sh | bash -s -- -b "{install_dir}" {" ".join(ver_flag)}'
    # Run through a shell because the script is a pipe
    r = run(
        cmd,
        cwd=str(repo_root),
        timeout=300,
        check=True,
        capture_output=False,
        env=os.environ | {"CI": "false"},
    )
    act_path = install_dir / "act"
    if not act_path.exists():
        raise RuntimeError("act installation completed but binary not found.")
    return act_path


def docker_image_exists(image: str) -> bool:
    r = run(["docker", "image", "inspect", image], capture_output=True)
    return r.returncode == 0


def docker_pull_with_retries(image: str, retries: int, timeout_s: int) -> bool:
    for i in range(1, retries + 1):
        print(f"  docker pull attempt {i}/{retries}: {image}", flush=True)
        try:
            run(["docker", "pull", image], timeout=timeout_s, check=True)
            return True
        except subprocess.TimeoutExpired:
            print("    timed out", flush=True)
        except subprocess.CalledProcessError as e:
            print(f"    failed with code {e.returncode}", flush=True)
        time.sleep(2 ** min(i, 6))
    return False


def select_runner_image(
    primary: str, alternates: list[str], retries: int, timeout_s: int, prepull: bool
) -> str | None:
    images = [primary] + [img for img in alternates if img]
    # If cached, use immediately
    for img in images:
        if docker_image_exists(img):
            return img
    if not prepull:
        # Let act pull implicitly
        return primary
    print(f"Pre-pulling runner images (will try: {' '.join(images)}) ...", flush=True)
    for img in images:
        if docker_pull_with_retries(img, retries=retries, timeout_s=timeout_s):
            return img
    return None


def main():
    ap = argparse.ArgumentParser(
        description="Run a GitHub Actions workflow locally via act."
    )
    ap.add_argument(
        "--workflow-yml",
        required=True,
        help="Relative path to workflow YAML (from --repo-root).",
    )
    ap.add_argument(
        "--repo-root", required=True, help="Root of the repository (filesystem path)."
    )

    # Optional quality-of-life flags
    ap.add_argument(
        "--event", default="push", help="GitHub event to simulate (default: push)."
    )
    ap.add_argument("--job", default="", help="Optionally run a single job by name.")
    ap.add_argument(
        "--act-version", default="", help="Pin act version, e.g. v0.2.82 (optional)."
    )
    ap.add_argument(
        "--act-install-dir", default="", help="Install dir for act if not found."
    )
    ap.add_argument(
        "--act-flags",
        default="",
        help="Extra flags to pass to act (string, parsed with shlex).",
    )

    ap.add_argument(
        "--runner-image",
        default="ghcr.io/catthehacker/ubuntu:act-22.04",
        help="Image to map to ubuntu-latest (default: ghcr.io/...:act-22.04).",
    )
    ap.add_argument(
        "--alt-images",
        default="ghcr.io/catthehacker/ubuntu:full-22.04 docker.io/catthehacker/ubuntu:full-22.04",
        help="Space- or comma-separated fallback images.",
    )
    ap.add_argument(
        "--prepull",
        action="store_true",
        default=True,
        help="Pre-pull runner images (default: True).",
    )
    ap.add_argument(
        "--no-prepull",
        dest="prepull",
        action="store_false",
        help="Disable pre-pulling images.",
    )
    ap.add_argument(
        "--pull-retries", type=int, default=3, help="Retries per image (default: 3)."
    )
    ap.add_argument(
        "--pull-timeout",
        type=int,
        default=900,
        help="Seconds to wait per pull (default: 900; 0 disables).",
    )

    ap.add_argument(
        "--secrets-file",
        default=os.environ.get("GHA_SECRETS_FILE", ""),
        help="Optional path to a secrets file for act (--secret-file).",
    )
    ap.add_argument(
        "--env-file",
        default=os.environ.get("GHA_ENV_FILE", ""),
        help="Optional path to an env file for act (--env-file).",
    )

    args = ap.parse_args()

    repo_root = Path(args.repo_root).resolve()
    if not repo_root.exists():
        print(f"ERROR: --repo-root does not exist: {repo_root}", file=sys.stderr)
        return 2

    workflow_path = (repo_root / args.workflow_yml).resolve()
    if not workflow_path.exists():
        print(f"ERROR: workflow file not found: {workflow_path}", file=sys.stderr)
        return 2

    ensure_docker_running()

    install_dir = Path(args.act_install_dir).resolve() if args.act_install_dir else None
    act_bin = find_or_install_act(repo_root, install_dir, args.act_version or None)
    print(f"Using act at: {act_bin}")

    alt_images = [s for s in shlex.split(args.alt_images.replace(",", " ")) if s]
    pull_timeout = None if args.pull_timeout == 0 else args.pull_timeout
    use_image = select_runner_image(
        args.runner_image,
        alt_images,
        retries=args.pull_retries,
        timeout_s=(pull_timeout or 10**9),
        prepull=args.prepull,
    )
    if not use_image:
        print("ERROR: Could not pull any runner image.", file=sys.stderr)
        return 3
    print(f"Using runner image: {use_image}")

    cmd = [
        str(act_bin),
        args.event,
        "-W",
        str(workflow_path),
        "-P",
        f"ubuntu-latest={use_image}",
    ]

    if args.job:
        cmd += ["-j", args.job]

    if args.secrets_file:
        cmd += ["--secret-file", args.secrets_file]
    if args.env_file:
        cmd += ["--env-file", args.env_file]

    if args.act_flags:
        cmd += shlex.split(args.act_flags)

    # Ensure PATH includes install dir for sub-processes if needed
    env = os.environ.copy()
    env["PATH"] = f"{act_bin.parent}:{env.get('PATH','')}"

    print("Executing:", " ".join(shlex.quote(c) for c in cmd), flush=True)
    try:
        r = run(cmd, cwd=str(repo_root), timeout=None, check=False, env=env)
        return r.returncode
    except KeyboardInterrupt:
        return 130


if __name__ == "__main__":
    sys.exit(main())
