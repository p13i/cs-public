#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <npm-script> [args...]" >&2
  exit 1
fi

CMD="$1"
shift

SOURCE="${BASH_SOURCE[0]}"
while [[ -h "$SOURCE" ]]; do
  DIR="$(cd -P "$(dirname "$SOURCE")" && pwd)"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
SCRIPT_DIR="$(cd -P "$(dirname "$SOURCE")" && pwd)"
APP_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

if [[ ! -d "${APP_DIR}/node_modules" ]]; then
  cat >&2 <<'MSG'
[mobile] Missing node_modules/. Run `npm install` inside cs/apps/mobile before
invoking the Bazel targets so the React Native toolchain is available.
MSG
  exit 4
fi

export CI="${CI:-1}"
cd "${APP_DIR}"

echo "[mobile] npm run ${CMD} $*"
npm run "${CMD}" -- "$@"
