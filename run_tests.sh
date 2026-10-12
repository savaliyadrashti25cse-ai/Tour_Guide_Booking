#!/usr/bin/env bash
set -eu
cd -- "$(dirname -- "$0")"
task_sources=()
for task_file in Source/*.cpp; do
  if [[ "$task_file" != "Source/main.cpp" ]]; then task_sources+=("$task_file"); fi
done
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 -ISource "${task_sources[@]}" Tests/system_tests.cpp -o Tests/system_tests
./Tests/system_tests
