#!/bin/bash
set -euo pipefail

# Install docker-cli at container start if missing, then exec the passed command.
if ! command -v docker >/dev/null 2>&1; then
  apt-get update
  apt-get install -y --no-install-recommends docker.io
  rm -rf /var/lib/apt/lists/*
fi

exec "$@"

