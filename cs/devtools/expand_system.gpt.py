#!/usr/bin/env python3
# cs/devtools/expand_system.gpt.py
"""
Expand system.gpt.yml infrastructure-as-code specification into docker-compose.yml
and a multi-stage Dockerfile.

This allows manipulating only the abstract system.gpt.yml without touching docker configs.
"""

import yaml
import sys
import hashlib
import subprocess
from pathlib import Path
from typing import Any, Dict, List, Optional, Set


def load_system_spec(path: Path) -> Dict[str, Any]:
    """Load and parse the system specification YAML."""
    with open(path) as f:
        return yaml.safe_load(f)


def get_git_files(directory: Path) -> Set[Path]:
    """Get all git-tracked files in a directory."""
    try:
        result = subprocess.run(
            ["git", "ls-files", str(directory)],
            cwd=directory.parent if directory.is_file() else directory,
            capture_output=True,
            text=True,
            check=False,
        )
        if result.returncode != 0:
            return set()
        files = set()
        for line in result.stdout.strip().split("\n"):
            if line:
                # Handle both absolute and relative paths
                file_path = Path(line)
                if not file_path.is_absolute():
                    file_path = (
                        directory.parent / line
                        if directory.is_file()
                        else directory.parent.parent / line
                    )
                if file_path.exists():
                    files.add(file_path)
        return files
    except Exception:
        return set()


def compute_directory_hash(directory: Path) -> str:
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
            try:
                rel_path = file_path.relative_to(directory)
            except ValueError:
                continue
            hasher.update(str(rel_path).encode())

            # Add file content to hash
            try:
                with open(file_path, "rb") as f:
                    hasher.update(f.read())
            except (IOError, PermissionError):
                pass

    return hasher.hexdigest()[:12]


def get_all_source_files(root_path: Path, exclude_apps: List[str] = None) -> Set[Path]:
    """
    Get all source files from the cs/ directory, excluding other app directories.
    This captures shared libraries that services depend on.
    """
    if exclude_apps is None:
        exclude_apps = []

    files = set()
    cs_dir = root_path / "cs"

    if not cs_dir.exists():
        return files

    # Get all git-tracked files in cs/
    try:
        result = subprocess.run(
            ["git", "ls-files", "cs/"],
            cwd=root_path,
            capture_output=True,
            text=True,
            check=False,
        )

        if result.returncode == 0:
            for line in result.stdout.strip().split("\n"):
                if not line:
                    continue

                file_path = root_path / line

                # Skip files in excluded app directories
                should_exclude = False
                for app in exclude_apps:
                    if f"cs/apps/{app}/" in str(file_path):
                        should_exclude = True
                        break

                if not should_exclude and file_path.exists() and file_path.is_file():
                    files.add(file_path)
    except Exception:
        pass

    return files


def compute_service_hash(
    service_name: str,
    service_config: Dict[str, Any],
    root_path: Path,
    all_service_names: List[str],
) -> str:
    """
    Compute a content hash for a service based on:
    - Service's own source files
    - ALL shared source files in cs/ (excluding other apps)
    - Service configuration in system.gpt.yml

    This ensures changes to shared libraries (like cs/net/proto/db/db.hh)
    correctly trigger rebuilds of all dependent services.
    """
    hasher = hashlib.sha256()

    # 1. Hash service's own directory
    source_path = service_config.get("source", "")
    if source_path:
        service_dir = root_path / source_path
        if service_dir.exists():
            dir_hash = compute_directory_hash(service_dir)
            hasher.update(dir_hash.encode())

    # 2. Hash all shared code (cs/* excluding other apps)
    # Extract app name from source path (e.g., cs/apps/trycopilot.ai -> trycopilot.ai)
    current_app = source_path.split("/")[-1] if source_path else ""
    other_apps = [app for app in all_service_names if app != service_name]

    # Get all source files but exclude other app directories
    shared_files = get_all_source_files(root_path, exclude_apps=other_apps)

    # Hash shared files (sorted for determinism)
    for file_path in sorted(shared_files):
        try:
            rel_path = file_path.relative_to(root_path)
            hasher.update(str(rel_path).encode())

            with open(file_path, "rb") as f:
                hasher.update(f.read())
        except (IOError, PermissionError, ValueError):
            pass

    # 3. Hash service configuration
    config_str = str(sorted(service_config.items()))
    hasher.update(config_str.encode())

    # 4. Hash service name
    hasher.update(service_name.encode())

    return hasher.hexdigest()[:8]


def check_image_exists(image_tag: str) -> bool:
    """Check if a docker image with the given tag exists locally."""
    try:
        result = subprocess.run(
            ["docker", "images", "-q", image_tag],
            capture_output=True,
            text=True,
            check=False,
        )
        return result.returncode == 0 and bool(result.stdout.strip())
    except Exception:
        return False


def get_replicas(service: Dict[str, Any], env: str) -> int:
    """Return replica count from service.replicas (int or env-to-int dict). Default 1."""
    replicas = service.get("replicas", 1)
    if isinstance(replicas, int):
        return replicas
    if isinstance(replicas, dict):
        return replicas.get(env, replicas.get("prod", 1))
    return 1


def get_user_id(security: Dict[str, Any]) -> str:
    """Convert abstract isolation to user ID."""
    isolation = security.get("isolation", "unprivileged")

    mapping = {
        "unprivileged": "8877:8877",
        "privileged": "0:0",
        "user-namespaced": "${UID}:${GID}",
    }

    return mapping.get(isolation, "8877:8877")


def get_network_config(network: Dict[str, Any], env: str) -> Dict[str, Any]:
    """Convert abstract network config to docker networking."""
    interface = network.get("interface", "private")
    port = network.get("port")

    if isinstance(interface, dict):
        interface = interface.get(env, interface.get("prod", "private"))

    config = {}

    if port:
        if interface in ["public", "management"]:
            config["ports"] = [f"{port}:{port}"]
        else:
            config["expose"] = [str(port)]

    return config


def map_storage_volumes(
    storage_list: List[Dict[str, Any]], storage_defs: Dict[str, Any]
) -> List[str]:
    """Convert abstract storage mounts to docker volume syntax."""
    volumes = []

    storage_name_map = {
        "database-volume": "database-volume",
    }

    for mount in storage_list:
        storage_name = mount["mount"]
        access = mount.get("access", "read-write")
        docker_vol = storage_name_map.get(storage_name, storage_name)

        storage_def = storage_defs.get(storage_name, {})
        mount_path = f"/data/{storage_name.replace('-', '_')}"

        if storage_name == "database-volume":
            mount_path = "/data"

        vol_str = f"{docker_vol}:{mount_path}"
        if access == "read-only":
            vol_str += ":ro"

        volumes.append(vol_str)

    return volumes


def get_service_command(service_name: str, service: Dict[str, Any]) -> List[str]:
    """Generate command array for service."""
    # Check if command is specified in YAML config
    if "command" in service:
        return service["command"]

    # Fall back to hardcoded logic for backward compatibility
    port = service.get("network", {}).get("port", 8080)
    config_vars = service.get("config", [])

    cmd = ["/main"]

    if service_name == "database-service":
        cmd.extend([f"--host=0.0.0.0", "--data_dir=/data", f"--port={port}"])
    elif service_name == "service-registry":
        cmd.extend([f"--host=0.0.0.0", f"--port={port}"])
    elif service_name == "load-balancer":
        storage = service.get("storage", [])
        has_balancer_state = any(
            s.get("type") == "balancer-state" or s == "balancer-state"
            for s in (storage if isinstance(storage, list) else [storage])
        )
        if has_balancer_state:
            script = (
                "chown -R myappuser:myappuser /data/load_balancer && "
                f"exec /main --host=0.0.0.0 --port={port}"
            )
            return ["bash", "-lc", script]
        cmd.extend([f"--host=0.0.0.0", f"--port={port}"])
    elif service_name in [
        "www-trycopilot-ai",
        "code-viewer",
        "www-cite-pub",
    ]:
        cmd.extend([f"--host=0.0.0.0", f"--port={port}"])
        if "VERSION" in config_vars:
            cmd.append("--version=${VERSION}")
        if "COMMIT" in config_vars:
            cmd.append("--commit=${COMMIT}")

    return cmd


def get_depends_on(dependencies: List[Any]) -> Dict[str, Any]:
    """Convert abstract dependencies to docker depends_on."""
    if not dependencies:
        return {}

    depends = {}
    for dep in dependencies:
        if isinstance(dep, str):
            depends[dep] = {"condition": "service_started"}
        elif isinstance(dep, dict):
            service = dep["service"]
            condition = "service_started"
            if dep.get("requirement") == "available":
                condition = "service_started"
            depends[service] = {"condition": condition}

    return depends


def generate_dockerfile(spec: Dict[str, Any], output_path: Path):
    """Generate multi-stage Dockerfile for all services."""
    services = spec.get("services", {})

    ubuntu_digest = "docker.io/library/ubuntu:24.04@sha256:59a458b76b4e8896031cd559576eac7d6cb53a69b38ba819fb26518536368d86"
    lines = [
        "# WARNING: This file is auto-generated from system.gpt.yml",
        "# Do not edit manually! Regenerate with: make expand",
        "#",
        "# Multi-stage Dockerfile - builds all services in parallel",
        "# Each service gets its own build stage and minimal runtime stage",
        "",
        "# Base build image with all source code and build tools (from context)",
        f"FROM {ubuntu_digest} AS base",
        "",
        "# Install build-time deps for Bazel/bazelisk (npx), C++ toolchain (rules_cc), and codegen (clang-format)",
        "RUN set -eux; \\",
        "    apt-get update; \\",
        "    apt-get install --yes --no-install-recommends \\",
        "    build-essential \\",
        "    ca-certificates \\",
        "    clang-format \\",
        "    nodejs \\",
        "    npm \\",
        "    && rm -rf /var/lib/apt/lists/*;",
        "",
        "# Create non-root user and cache dirs for Bazel/bazelisk (matching downstream UID/GID)",
        "RUN groupadd -g 8877 myappuser \\",
        "    && useradd -m -u 8877 -g 8877 -s /bin/bash myappuser \\",
        "    && mkdir -p /home/myappuser/.cache/bazel /home/myappuser/.cache/bazelisk \\",
        "    && chown -R myappuser:myappuser /home/myappuser/.cache",
        "",
        "# Copy repo into image so base reflects current workspace",
        "COPY --chown=myappuser:myappuser . /app",
        "WORKDIR /app",
        "USER myappuser",
        "",
        "# Minimal runtime base image",
        f"FROM {ubuntu_digest} AS runtime-base",
        "",
        "# Install only runtime dependencies",
        "RUN set -eux; \\",
        "    apt-get update; \\",
        "    apt-get install --yes --no-install-recommends \\",
        "    ca-certificates \\",
        "    libc6 \\",
        "    libstdc++6 \\",
        "    libgcc-s1 \\",
        "    && rm -rf /var/lib/apt/lists/*;",
        "",
        "# Create non-root user (matching base image UID/GID)",
        "RUN groupadd -g 8877 myappuser \\",
        "    && useradd -m -u 8877 -g 8877 -s /bin/bash myappuser",
        "",
        "# Create /data directory",
        "RUN mkdir -p /data \\",
        "    && chown -R myappuser:myappuser /data \\",
        "    && chmod 755 /data",
        "",
        "WORKDIR /app",
        "USER myappuser",
        "",
    ]

    # Generate build stage for each service
    for service_name, service in services.items():
        if service.get("type") == "tunnel":
            continue

        source_path = service.get("source", "")
        if not source_path:
            continue

        stage_name = service_name.replace("-", "_")
        bazel_target = f"//{source_path}:main"
        output_name = f"bazel-output-{stage_name}"

        lines.extend(
            [
                f"# Build stage for {service_name}",
                f"FROM base AS build_{stage_name}",
                "USER myappuser",
                "WORKDIR /app",
                "RUN --mount=type=cache,target=/home/myappuser/.cache/bazel,uid=8877,gid=8877 \\",
                "    mkdir -p /home/myappuser/.cache && \\",
                "    mkdir -p /home/myappuser/.cache/bazelisk && \\",
                "    USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk \\",
                f"    --output_base=/tmp/{output_name} \\",
                f"    build --config=release {bazel_target}",
                "",
            ]
        )

    # Generate runtime stages
    lines.extend(
        [
            "# Minimal runtime base",
            "FROM runtime-base AS runtime",
            "",
            "# cs-public stage: clone repo, strip .git (baked into code-viewer image)",
            "FROM alpine/git AS cs-public",
            "RUN git clone --depth 1 https://github.com/p13i/cs-public /cs-public && rm -rf /cs-public/.git",
            "",
        ]
    )

    for service_name, service in services.items():
        if service.get("type") == "tunnel":
            continue

        source_path = service.get("source", "")
        if not source_path:
            continue

        stage_name = service_name.replace("-", "_")
        port = service.get("network", {}).get("port", 8080)
        security = service.get("security", {})
        user = get_user_id(security)

        # For user-namespaced, default to myappuser in Dockerfile
        if user == "${UID}:${GID}":
            dockerfile_user = "myappuser"
        elif user == "0:0":
            dockerfile_user = "root"
        else:
            dockerfile_user = "myappuser"

        binary_source = f"/app/bazel-bin/{source_path}/main"

        cmd_list = get_service_command(service_name, service)
        cmd_json = str(cmd_list).replace("'", '"')

        lines.extend(
            [
                f"# Runtime stage for {service_name}",
                f"FROM runtime AS {stage_name}",
            ]
        )

        # Special case: service-registry needs docker CLI
        if service_name == "service-registry":
            lines.extend(
                [
                    "# Install docker CLI for service discovery",
                    "USER root",
                    "RUN apt-get update && apt-get install -y --no-install-recommends docker.io && rm -rf /var/lib/apt/lists/*",
                ]
            )

        copy_lines = [
            "WORKDIR /app",
            f"EXPOSE {port}",
            f"COPY --from=build_{stage_name} --chown={dockerfile_user}:{dockerfile_user} \\",
            f"    {binary_source} /main",
        ]
        if service_name == "code-viewer":
            copy_lines.extend(
                [
                    f"COPY --from=cs-public --chown={dockerfile_user}:{dockerfile_user} \\",
                    "    /cs-public /cs-public",
                ]
            )
        lines.extend(copy_lines)

        lines.extend(
            [
                f"USER {dockerfile_user}",
                f"CMD {cmd_json}",
                "",
            ]
        )

    # Special case for tunnel
    lines.extend(
        [
            "# Tunnel service uses ngrok base image",
            "FROM ngrok/ngrok:latest AS ngrok",
            "# Configuration provided via docker-compose entrypoint",
            "",
        ]
    )

    output_path.write_text("\n".join(lines))
    print(f"Generated {output_path}")


def generate_compose(
    spec: Dict[str, Any], output_path: Path, env: str = "prod", root_path: Path = None
):
    """Generate docker-compose.yml for specified environment."""
    services = spec.get("services", {})
    storage_defs = spec.get("storage", {})

    # Compute service hashes for intelligent caching
    if root_path is None:
        root_path = output_path.parent

    service_hashes = {}
    all_service_names = list(services.keys())
    for service_name, service_config in services.items():
        if service_config.get("type") != "tunnel":
            service_hashes[service_name] = compute_service_hash(
                service_name, service_config, root_path, all_service_names
            )

    compose = {
        "services": {},
        "volumes": {},
    }

    for service_name, service in services.items():
        service_type = service.get("type", "web-application")

        # Handle tunnel specially
        if service_type == "tunnel":
            # Get expose configuration (which services/domains to tunnel)
            expose_config = service.get("expose", {})
            entrypoint_script = generate_ngrok_entrypoint(expose_config, env, services)

            restart_policy = service.get("restart_policy", "no")
            if restart_policy == "manual":
                restart_policy = "no"

            svc = {
                "image": "ngrok/ngrok:latest",
                "container_name": f"cs-{service_name}",
                "env_file": ".env",
                "entrypoint": ["/bin/sh", "-c", entrypoint_script],
                "restart": restart_policy,
            }

            # Management port from network config
            mgmt_port = service.get("network", {}).get("management_port", 4040)
            if mgmt_port:
                svc["ports"] = [f"{mgmt_port}:{mgmt_port}"]

            svc["extra_hosts"] = ["host.docker.internal:host-gateway"]

            deps = service.get("dependencies", [])
            if deps:
                svc["depends_on"] = get_depends_on(deps)

            compose["services"][service_name] = svc
            continue

        # Regular services
        stage_name = service_name.replace("-", "_")

        # Generate image tag with content hash for intelligent caching
        service_hash = service_hashes.get(service_name, "unknown")
        image_tag = f"cs-app:{service_name}-{service_hash}"

        # Check if image already exists to enable cache reuse
        image_exists = check_image_exists(image_tag)

        svc = {
            "build": {
                "context": ".",
                "dockerfile": "Dockerfile",
                "target": stage_name,
                # "cache_from": [image_tag] if image_exists else [],
            },
            "image": image_tag,
            "pull_policy": "never",
            "env_file": ".env",
            "restart": "unless-stopped",
        }

        # Add labels for metadata
        svc["labels"] = {
            "cs.service.name": service_name,
            "cs.service.hash": service_hash,
        }

        # Container name if needed
        if get_replicas(service, env) == 1:
            container_name = service_name
            if service_name in ["www-cite-pub", "service-registry"]:
                svc["container_name"] = f"cs-{container_name}"

        # User
        security = service.get("security", {})
        user = get_user_id(security)
        svc["user"] = user

        # Replicas
        replicas = get_replicas(service, env)
        if replicas > 1:
            svc["deploy"] = {"replicas": replicas}

        # Network - handle environment-specific port exposure
        network = service.get("network", {})
        interface = network.get("interface", "private")
        port = network.get("port")

        # Handle environment-specific interface config
        if isinstance(interface, dict):
            interface = interface.get(env, interface.get("prod", "private"))

        if port:
            if service_name == "load-balancer" and env == "dev":
                # Load balancer exposed in dev
                svc["ports"] = [f"{port}:{port}"]
            elif interface in ["public", "management"]:
                svc["ports"] = [f"{port}:{port}"]
            elif service_name == "load-balancer":
                # Load balancer internal-only in prod
                svc["expose"] = [str(port)]
            else:
                svc["expose"] = [str(port)]

        # Volumes
        storage_list = service.get("storage", [])
        if storage_list:
            svc["volumes"] = map_storage_volumes(storage_list, storage_defs)

        # Command
        cmd = get_service_command(service_name, service)
        if service_name == "load-balancer":
            cmd = list(cmd)
            if len(cmd) == 3 and cmd[0] == "bash":
                cmd[2] = cmd[2].replace(
                    "exec /main ", f"exec /main --environment={env} "
                )
            else:
                cmd = cmd + [f"--environment={env}"]
        if cmd != ["/main"]:
            svc["command"] = cmd

        # Environment variables
        if service_name == "service-registry":
            svc["environment"] = ["COMPOSE_PROJECT_NAME=${COMPOSE_PROJECT_NAME:-cs}"]
            # Add docker socket mount
            if "volumes" not in svc:
                svc["volumes"] = []
            svc["volumes"].append("/var/run/docker.sock:/var/run/docker.sock:ro")

        # Dependencies
        deps = service.get("dependencies", [])
        if deps:
            depends = get_depends_on(deps)
            if depends:
                svc["depends_on"] = depends

        compose["services"][service_name] = svc

    # Add volumes
    for storage_name, storage_def in storage_defs.items():
        docker_vol = {
            "database-volume": "database-volume",
        }.get(storage_name, storage_name)

        vol_config = {}
        # Only database-volume is external (backed up)
        if storage_name == "database-volume":
            vol_config["external"] = True

        compose["volumes"][docker_vol] = vol_config if vol_config else None

    # Write with header comment
    header = f"""# WARNING: This file is auto-generated from system.gpt.yml
# Do not edit manually! Regenerate with: make expand
# Environment: {env}

"""
    # Serialize to YAML (docker-compose requires YAML format)
    yaml_content = yaml.dump(compose, default_flow_style=False, sort_keys=False)
    output_path.write_text(header + yaml_content)
    print(f"Generated {output_path}")


def generate_ngrok_entrypoint(
    expose_config: Dict[str, Any], env: str, services_config: Dict[str, Any]
) -> str:
    """Generate ngrok entrypoint script from simplified expose configuration."""
    config_lines = [
        "version: '3'",
        "agent:",
        "  authtoken: 382hPPpjmtecc79LYbukRGmD2o3_GArhu6V5osRWjAhSNkfc",
        "tunnels:",
    ]

    tunnel_names = []

    # Process each service that needs to be exposed
    for service_name, domains_config in expose_config.items():
        # Get domain list for this environment
        if isinstance(domains_config, dict):
            domain_list = domains_config.get(env, [])
        else:
            domain_list = domains_config if isinstance(domains_config, list) else []

        if not domain_list:
            continue

        # Determine service address from service configuration
        service_info = services_config.get(service_name, {})
        service_port = service_info.get("network", {}).get("port", 8080)
        addr = f"{service_name}:{service_port}"

        # Create tunnel for each domain
        for domain in domain_list:
            tunnel_name = domain.replace(".", "-")
            config_lines.extend(
                [
                    f"  {tunnel_name}:",
                    f"    proto: http",
                    f"    addr: {addr}",
                    f"    domain: {domain}",
                ]
            )
            tunnel_names.append(tunnel_name)

    config_file = "/tmp/ngrok.yml"

    script = f"""cat >{config_file} <<'EOF'
{chr(10).join(config_lines)}
EOF
exec ngrok start --config {config_file} {' '.join(tunnel_names)}"""

    return script


def main():
    if len(sys.argv) < 2:
        print("Usage: expand_system.gpt.py <system.gpt.yml>")
        sys.exit(1)

    system_file = Path(sys.argv[1])
    if not system_file.exists():
        print(f"Error: {system_file} not found")
        sys.exit(1)

    spec = load_system_spec(system_file)

    root = system_file.parent

    # Generate Dockerfile
    dockerfile_path = root / "Dockerfile"
    generate_dockerfile(spec, dockerfile_path)

    # Generate docker-compose.yml (production)
    compose_path = root / "docker-compose.yml"
    generate_compose(spec, compose_path, env="prod", root_path=root)

    # Generate docker-compose.dev.yml
    compose_dev_path = root / "docker-compose.dev.yml"
    generate_compose(spec, compose_dev_path, env="dev", root_path=root)

    print("\nDone! Generated:")
    print(f"  - {dockerfile_path}")
    print(f"  - {compose_path}")
    print(f"  - {compose_dev_path}")
    print("\nNote: routing.json is generated by Bazel at build time.")
    print("You can now modify only system.gpt.yml and regenerate these files.")


if __name__ == "__main__":
    main()
