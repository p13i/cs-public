#!/usr/bin/env bash
set -euo pipefail

cleanup() {
  [[ -n "${WORKDIR:-}" && -d "${WORKDIR}" ]] \
    && rm -rf "${WORKDIR}"
}
trap cleanup EXIT

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKDIR="$(mktemp -d)"
SOURCE="${ROOT}/hello_world.gpt.sh"

cp "${SOURCE}" "${WORKDIR}/"
pushd "${WORKDIR}" >/dev/null
chmod +x hello_world.gpt.sh
output="$(./hello_world.gpt.sh | tr -d '\r\n')"
popd >/dev/null

if [[ "${output}" != "Hello World!" ]]; then
  echo "unexpected output: '${output}'" >&2
  exit 1
fi

echo "Bash Hello World produced: ${output}"
