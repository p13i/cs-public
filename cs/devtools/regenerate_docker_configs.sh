#!/bin/bash
set -euo pipefail

# Regenerate docker-compose and Dockerfile from system.gpt.yml
# Usage: bazel run //cs/devtools:regenerate_docker_configs
# Or simply: make expand

WORKSPACE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$WORKSPACE_ROOT"

echo "Regenerating docker configs from system.gpt.yml..."
USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk run //cs/devtools:expand_system -- "$WORKSPACE_ROOT/system.gpt.yml"

echo ""
echo "Running validation tests..."
USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk run //cs/devtools:test_expand_system -- "$WORKSPACE_ROOT"

echo ""
echo "✅ Done! Generated files:"
echo "  - Dockerfile"
echo "  - docker-compose.yml"
echo "  - docker-compose.dev.yml"
echo ""
echo "To use these configs:"
echo "  docker compose up                                    # production"
echo "  docker compose -f docker-compose.yml -f docker-compose.dev.yml up  # development"
