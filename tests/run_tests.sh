#!/usr/bin/env bash
set -e
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="${TMPDIR:-/tmp}/transit_planner_tests"
mkdir -p "$TMP"
for source in "$ROOT"/tests/test_*.cpp; do
  name=$(basename "$source" .cpp)
  echo "[BUILD] $name"
  g++ -std=c++11 -Wall -Wextra -pedantic "$source" -o "$TMP/$name"
  echo "[TEST]  $name"
  "$TMP/$name"
done
echo "All tests passed."
