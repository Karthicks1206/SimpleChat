SimpleChat2 — UDP P2P + Broadcast (Qt6)
mkdir -p build && cd build
cmake ..
cmake --build . -j
ctest --output-on-failure
./scripts/run_many.sh 3

- QUdpSocket transport (chat + discovery)
- JSON-on-the-wire (`type, origin, seq, dest, text, ackOf, vc`)
- ACK + resend timers + bounded retries
- Vector clocks + anti-entropy sync
- Unit tests (message roundtrip, vector clock diff)
