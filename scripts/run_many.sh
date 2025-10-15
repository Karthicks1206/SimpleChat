#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
NUM=${1:-3}
BASE_CHAT=${2:-45454}
BASE_DISC=${3:-45455}
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
cd ..
for i in $(seq 0 $((NUM-1))); do
  p=$((BASE_CHAT+i))
  ./scripts/launch_instance.sh "$p" "$BASE_DISC"
done
echo "Started $NUM instances. Use Dest=-1 for broadcast."
