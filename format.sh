#!/usr/bin/env sh
set -e

if ! command -v clang-format >/dev/null 2>&1; then
  echo "clang-format not found"
  exit 1
fi

find src \
  -type f \( -name "*.cpp" -o -name "*.hpp" \) \
  -exec clang-format -i -style=file {} +
