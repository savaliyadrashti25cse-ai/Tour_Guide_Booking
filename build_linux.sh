#!/usr/bin/env bash
set -eu
cd -- "$(dirname -- "$0")"
mkdir -p Executable
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 Source/*.cpp -o Executable/Project
printf 'Build successful: Executable/Project\n'
