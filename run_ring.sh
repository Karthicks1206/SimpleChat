#!/usr/bin/env bash
set -euo pipefail
EXE="${EXE:-./build/SimpleChat}"
N="${1:-4}"
BASE="${2:-9001}"

if [ ! -x "$EXE" ]; then
  echo "ERROR: $EXE not found. Build first (cmake --build build)." >&2
  exit 1
fi

echo "Launching $N nodes starting at port $BASE ..."
pids=()
for ((i=0;i<N;i++)); do
  port=$((BASE+i))
  right=$((BASE+((i+1)%N)))
  "$EXE" --id "$port" --port "$port" --right "$right" &>/dev/null &
  pids+=($!)
  sleep 0.1
done
echo "PIDs: ${pids[*]}"
wait
