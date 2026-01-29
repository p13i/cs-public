#!/usr/bin/env bash

say_hello() {
  local target="${1:-World}"
  echo "Hello ${target}!"
}

say_hello "$@"
