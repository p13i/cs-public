#!/usr/bin/env python3
# cs/devtools/compute_service_hash.gpt.py
"""
Compute content-based hashes for services to enable intelligent cache invalidation.

This allows docker-compose to skip rebuilding images when source hasn't changed.
"""

import hashlib
import subprocess
import sys
from pathlib import Path
from typing import Dict, Set, Any


def get_git_files(directory: Path) -> Set[Path]:
    """Get all git-tracked files in a directory."""
    try:
        result = subprocess.run(
            ["git", "ls-files", str(directory)],
            cwd=directory.parent if directory.is_file() else directory,
            capture_output=True,
            text=True,
            check=True,
        )
        files = set()
        for line in result.stdout.strip().split("\n"):
            if line:
                file_path = (
                    directory.parent / line if directory.is_file() else directory / line
                )
                if file_path.exists():
                    files.add(file_path)
        return files
    except subprocess.CalledProcessError:
        return set()


def compute_directory_hash(directory: Path, include_patterns: Set[str] = None) -> str:
    """Compute hash of all files in directory."""
    if not directory.exists():
        return hashlib.sha256(b"").hexdigest()[:12]

    hasher = hashlib.sha256()

    # Get git-tracked files for better accuracy
    files = get_git_files(directory)
    if not files:
        # Fallback to all files if not in git
        files = set(directory.rglob("*"))

    # Sort for deterministic hashing
    for file_path in sorted(files):
        if file_path.is_file():
            # Add relative path to hash
            rel_path = (
                file_path.relative_to(directory)
                if file_path.is_relative_to(directory)
                else file_path
            )
            hasher.update(str(rel_path).encode())

            # Add file content to hash
            try:
                with open(file_path, "rb") as f:
                    hasher.update(f.read())
            except (IOError, PermissionError):
                pass

    return hasher.hexdigest()[:12]


def compute_service_hash(
    service_name: str, service_config: Dict[str, Any], root_path: Path
) -> str:
    """
    Compute a content hash for a service based on:
    - Source files in the service directory
    - Service configuration in system3.gpt.yml
    - Base image dependencies
    """
    hasher = hashlib.sha256()

    # 1. Hash service source directory
    source_path = service_config.get("source", "")
    if source_path:
        service_dir = root_path / source_path
        if service_dir.exists():
            dir_hash = compute_directory_hash(service_dir)
            hasher.update(dir_hash.encode())

    # 2. Hash service configuration (affects build)
    config_str = str(sorted(service_config.items()))
    hasher.update(config_str.encode())

    # 3. Hash service name
    hasher.update(service_name.encode())

    # 4. Include base image info if it's a native build
    if service_config.get("runtime") == "native-binary":
        hasher.update(b"cs-app-base:latest")
        hasher.update(b"runtime-base:latest")

    return hasher.hexdigest()[:12]


def get_git_commit() -> str:
    """Get current git commit SHA (short)."""
    try:
        result = subprocess.run(
            ["git", "rev-parse", "--short=8", "HEAD"],
            capture_output=True,
            text=True,
            check=True,
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError:
        return "unknown"


def compute_all_service_hashes(spec: Dict[str, Any], root_path: Path) -> Dict[str, str]:
    """Compute hashes for all services."""
    services = spec.get("services", {})
    hashes = {}

    for service_name, service_config in services.items():
        if service_config.get("type") == "tunnel":
            # Tunnel uses third-party image, no custom build
            hashes[service_name] = "external"
        else:
            hashes[service_name] = compute_service_hash(
                service_name, service_config, root_path
            )

    return hashes


def check_image_exists(image_tag: str) -> bool:
    """Check if a docker image with the given tag exists locally."""
    try:
        result = subprocess.run(
            ["docker", "images", "-q", image_tag],
            capture_output=True,
            text=True,
            check=True,
        )
        return bool(result.stdout.strip())
    except subprocess.CalledProcessError:
        return False


def main():
    """CLI for computing service hashes."""
    if len(sys.argv) < 2:
        print("Usage: compute_service_hash.gpt.py <service-name> [root-path]")
        sys.exit(1)

    service_name = sys.argv[1]
    root_path = Path(sys.argv[2]) if len(sys.argv) > 2 else Path.cwd()

    # For now, just compute based on directory
    service_dir = root_path / "cs" / "apps" / service_name
    if service_dir.exists():
        hash_val = compute_directory_hash(service_dir)
        print(f"{service_name}: {hash_val}")
    else:
        print(f"Error: Service directory not found: {service_dir}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
