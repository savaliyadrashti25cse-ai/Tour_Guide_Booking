#!/usr/bin/env bash
set -eu
cd -- "$(dirname -- "$0")"
./Executable/Project --data-dir "$PWD/Data"
