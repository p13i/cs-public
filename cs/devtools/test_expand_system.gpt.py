#!/usr/bin/env python3
# cs/devtools/test_expand_system.gpt.py
"""
Test that expand_system.gpt.py generates valid and correct docker configs.
"""

import yaml
import sys
from pathlib import Path


def test_generated_files():
    """Verify generated files exist and have correct structure."""
    # When run via Bazel, use cwd. When run directly, use relative path.
    if len(sys.argv) > 1:
        root = Path(sys.argv[1])
    else:
        root = Path(__file__).parent.parent.parent

    # Check files exist
    dockerfile = root / "Dockerfile"
    compose_prod = root / "docker-compose.yml"
    compose_dev = root / "docker-compose.dev.yml"

    assert dockerfile.exists(), f"{dockerfile} not found"
    assert compose_prod.exists(), f"{compose_prod} not found"
    assert compose_dev.exists(), f"{compose_dev} not found"

    print("✓ All generated files exist")

    # Validate YAML
    with open(compose_prod) as f:
        prod = yaml.safe_load(f)

    with open(compose_dev) as f:
        dev = yaml.safe_load(f)

    print("✓ Generated YAML is valid")

    # Check structure
    assert "services" in prod, "Missing services in prod"
    assert "volumes" in prod, "Missing volumes in prod"

    assert "services" in dev, "Missing services in dev"
    assert "volumes" in dev, "Missing volumes in dev"

    print("✓ YAML structure is correct")

    # Verify all expected services
    expected_services = [
        "www-trycopilot-ai",
        "www-cite-pub",
        "code-viewer",
        "data-viewer",
        "database-service",
        "message-queue",
        "task-queue-service",
        "blob-service",
        "blob-viewer",
        "scribe-service",
        "scribe-worker",
        "load-balancer",
        "service-registry",
        "ngrok",
    ]

    for svc in expected_services:
        assert svc in prod["services"], f"Missing service {svc} in prod"
        assert svc in dev["services"], f"Missing service {svc} in dev"

    print(f"✓ All {len(expected_services)} services present")

    # Verify environment differences
    prod_replicas = (
        prod["services"]["www-trycopilot-ai"].get("deploy", {}).get("replicas", 1)
    )
    dev_replicas = (
        dev["services"]["www-trycopilot-ai"].get("deploy", {}).get("replicas", 1)
    )

    assert prod_replicas == 5, f"Expected 5 prod replicas, got {prod_replicas}"
    assert dev_replicas == 3, f"Expected 3 dev replicas, got {dev_replicas}"

    print(f"✓ Environment-specific scaling: prod={prod_replicas}, dev={dev_replicas}")

    # Verify load-balancer ports
    prod_lb = prod["services"]["load-balancer"]
    dev_lb = dev["services"]["load-balancer"]

    assert "expose" in prod_lb, "Load balancer should be internal in prod"
    assert "ports" in dev_lb, "Load balancer should be exposed in dev"

    print("✓ Load balancer correctly exposed in dev, internal in prod")

    # Verify data services use named volumes mounted at /data
    expected_volumes = {
        "database-service": "database-volume",
        "blob-service": "blob-volume",
        "scribe-worker": "scribe-workspace-volume",
        "data-viewer": "database-volume",
    }
    for svc_name, vol_name in expected_volumes.items():
        svc = prod["services"][svc_name]
        assert "volumes" in svc, f"{svc_name} should have volumes"
        data_mount = f"{vol_name}:/data"
        assert (
            data_mount in svc["volumes"]
        ), f"{svc_name} should have volume {data_mount}, got {svc['volumes']}"
        assert vol_name in prod.get(
            "volumes", {}
        ), f"{vol_name} should be declared in compose volumes"

    print(
        "✓ Data services use named volumes (database-volume, blob-volume, scribe-workspace-volume)"
    )

    # Verify security isolation
    assert (
        prod["services"]["database-service"]["user"] == "0:0"
    ), "database-service should run as root"
    assert (
        prod["services"]["www-trycopilot-ai"]["user"] == "8877:8877"
    ), "www-trycopilot-ai should be unprivileged"

    print("✓ Security isolation levels correct")

    # Verify Dockerfile has all build stages
    dockerfile_content = dockerfile.read_text()

    for svc in expected_services:
        if svc == "ngrok":
            continue
        stage_name = svc.replace("-", "_")
        assert (
            f"AS build_{stage_name}" in dockerfile_content
        ), f"Missing build stage for {svc}"
        assert (
            f"AS {stage_name}" in dockerfile_content
        ), f"Missing runtime stage for {svc}"

    print("✓ Dockerfile has all build and runtime stages")

    # Guard against regressing to per-stage builds: one shared build_full with multi-target bazel build
    assert "AS build_full" in dockerfile_content, "Missing shared build_full stage"
    build_release_lines = [
        line
        for line in dockerfile_content.splitlines()
        if "build --config=release" in line
    ]
    assert (
        len(build_release_lines) == 1
    ), f"Expected exactly one 'build --config=release' RUN; got {len(build_release_lines)}"
    assert (
        "//" in build_release_lines[0]
    ), "Shared build should list multiple // targets"

    print(
        "✓ Dockerfile uses single shared full build (build_full) with multi-target bazel build"
    )

    print("\n✅ All tests passed!")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(test_generated_files())
    except AssertionError as e:
        print(f"\n❌ Test failed: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"\n❌ Error: {e}")
        sys.exit(1)
