#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/.arduino/build"
DEFAULT_CLI="/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli"
FQBN="${ARDUINO_FQBN:-m5stack:esp32:m5stack_stickc_plus2}"

if [[ -n "${ARDUINO_CLI:-}" ]]; then
  CLI="${ARDUINO_CLI}"
elif command -v arduino-cli >/dev/null 2>&1; then
  CLI="$(command -v arduino-cli)"
elif [[ -x "${DEFAULT_CLI}" ]]; then
  CLI="${DEFAULT_CLI}"
else
  echo "arduino-cli was not found." >&2
  echo "Install it or set ARDUINO_CLI to its full path." >&2
  exit 1
fi

compile() {
  mkdir -p "${BUILD_DIR}"
  "${CLI}" compile \
    --fqbn "${FQBN}" \
    --build-path "${BUILD_DIR}" \
    "$@" \
    "${PROJECT_DIR}"
}

usage() {
  cat <<EOF
Usage:
  ./scripts/arduino.sh compile
  ./scripts/arduino.sh clean
  ./scripts/arduino.sh upload <serial-port>
  ./scripts/arduino.sh version

Environment overrides:
  ARDUINO_CLI   Full path to arduino-cli
  ARDUINO_FQBN  Board identifier (default: ${FQBN})
EOF
}

case "${1:-compile}" in
  compile)
    compile
    ;;
  clean)
    compile --clean
    ;;
  upload)
    if [[ -z "${2:-}" ]]; then
      echo "Missing serial port, for example /dev/cu.usbserial-0001." >&2
      usage >&2
      exit 2
    fi
    compile --upload --port "$2"
    ;;
  version)
    "${CLI}" version
    ;;
  help|-h|--help)
    usage
    ;;
  *)
    echo "Unknown command: $1" >&2
    usage >&2
    exit 2
    ;;
esac
