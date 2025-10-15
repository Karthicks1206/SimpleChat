#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
PORT=${1:-45454}
DISC=${2:-45455}
./build/simplechat2 "$PORT" "$DISC" &
echo "Launched simplechat2 on chat=$PORT disc=$DISC pid=$!"
