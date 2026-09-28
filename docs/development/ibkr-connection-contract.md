# Native broker connection contract

Umicom Framework — Sammy Hegab, Umicom Foundation — MIT

## Ownership and purpose

`Umicom::ibkr_connection` owns a bounded read-only TCP session. It composes the canonical `UmiIbkrAdapterConfig` and `UmiTradingEnvironment`. It does not establish a second OMS, ledger, identity provider or database. Trader only attaches its optional GTK view. Neither configuration nor opening a view grants trading authority.

The implementation is independently written C23. It negotiates legacy field protocol versions 151 through 176 and stops on an unsupported server response. No official SDK, Python runtime, C++ vendor source or protobuf runtime is bundled. Real-provider qualification is open. A successful synthetic peer test cannot close that gate.

## Lifetime

Create allocates a connection and an owned native I/O context, without opening a socket. Open may be called once per instance. All subsequent operations, including Copy and Destroy, must be serialised by the owner. Destroy closes the socket and releases memory. Callers must stop using the object before destruction; no implicit refcount or thread-safe concurrent Close is promised.

The GUI owns its connection on the GTK main context and polls nonblocking I/O. Destruction removes its timer and closes immediately. Widget callbacks use object-bound closures and check the closed state. The wrapper keeps a weak parent reference. The CLI is a single-owner polling client.

## Protocol and requests

Initial bytes: `API\0`, a network-order length and `v151..176`. The server response is a version and connection-time field. After negotiation the client sends startApi, waits for managed accounts and nextValidId, and asks for the current time. Accounts, readiness and the clock response are all required before the Ready state.

Only outbound message IDs 71 (startApi), 49 (time), 62/63 (request/cancel summary), 61/64 (request/cancel positions) exist in the private emitter. Handshake and request encoders never accept an arbitrary command or order payload. This allowlist is tested. Subscription cancellation does not cancel orders. The existing mapper remains a separate translation function and never gets called to send an order by this component.

Each instance permits one selected-account request. The summary request uses request ID 35001 and group All for four tags. The positions request is not correlated by a request ID in this profile. Therefore reusing it across multiple account reads is deliberately prohibited. Exact account filtering is local; provider requests may retrieve other authorised accounts transiently. Reconnecting creates a new object/socket and clears old buffers. Large adviser/on-demand-account profiles are not qualified.

Frames are network-order length followed by null-terminated fields. Bodies above 65,536 bytes are refused. Known messages require exact counts and supported versions. Unknown message identifiers are counted and ignored without decoding them as known records. Maximum field count is 32. UTF-8 text is checked and control characters are refused. Oversized text is rejected; oversized provider diagnostics are explicitly omitted rather than cut mid-codepoint.

Summary and position end markers complete independent snapshots. Initial subscriptions are then cancelled. The resulting observations are neither continuously updated nor an atomic portfolio. Raw provider number strings are retained without rounding or claiming canonical monetary authority. A missing value is not zero. Connection staleness is distinct from observation age.

## Environments and safety

Paper/Live is requested intent, never inferred from a port or account prefix. `environmentAttested` is always false. Live requires an explicit acknowledgement, but this is not authentication or order approval. Only 127.0.0.1 is permitted; client ID zero and remote/DNS hosts are rejected. Local TCP provides no peer authentication or encryption. The developer must trust and qualify the local endpoint and confirm the provider login.

The old mapper keeps valid enum/public API conventions. Additional guards reject non-finite numerical values, invalid sides/environments, unterminated fixed buffers and non-boolean policy flags before replacing output. This bounded mapper profile requires positive active limit/stop prices; negative-price products need an explicit instrument-aware policy rather than bypassing the checks.

## Failures and clocks

Use monotonic milliseconds. Backward time supplied by a caller is rejected. The GUI closes rather than retaining an unpumped socket after a pump error. Default handshake, request and heartbeat deadlines are 10 seconds. A clock probe is sent after 30 seconds since the last completed probe. There is no automatic reconnect or order retry.

Nonblocking transport may make short progress or return BUSY with zero progress. EOF, impossible byte counts, malformed frames, account-list changes and capacity errors close the session. Readiness and request operations keep previous observations visible but stale on failure. Provider farm notices 2104/2106/2107/2108/2158 are informational; other provider errors stop this limited inspection. This policy favours a visible refusal over unsupported automatic recovery.

## Bounds

32 accounts; 64 summary rows; 64 positions; 64 frames and 64 KiB read per pump. Internal send queue 8 KiB. Snapshot strings have published fixed capacities. Production code does not write files, database records or automatic logs. Console output, screenshots and dumps can still contain private account data.

## Qualification gates

Executed tests cover native Linux I/O into an inert loopback peer, not IBKR. Required before claiming provider readiness: actual TWS/Gateway version negotiation, login/managed accounts, Paper and separate Live read-only observations, request completion and disconnect. Windows adapter, GTK lifecycle and full Trader integration need actual target runs. No trade should be used as a connectivity smoke test.
