#!/usr/bin/env bash
set -euo pipefail
LAUNCH="${LAUNCH:-./run_ring.sh}"
INJECT="${INJECT:-./build/SimpleInjector}"

# optional: start the ring if not already running
if ! pgrep -x SimpleChat >/dev/null 2>&1; then
  "$LAUNCH" 4 9001 &
  sleep 1.5
fi

# Demo 1: ordering (send seq 2 then seq 1; receiver should show 1 then 2)
"$INJECT" --port 9001 --origin DEMO9001 --dest 9003 --seq 2 --text "(seq 2) should display SECOND"
sleep 0.12
"$INJECT" --port 9001 --origin DEMO9001 --dest 9003 --seq 1 --text "(seq 1) should display FIRST"

# Demo 2: propagation (multi-hop 9002 -> 9004)
"$INJECT" --port 9002 --origin DEMO9002 --dest 9004 --seq 1 --text "Propagation check 9002->9004"

echo "Automated tests sent. Check windows 9003 (ordering) and 9004 (propagation)."
