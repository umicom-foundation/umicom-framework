# Focused risk review — Financial close review and Winsock boundary

## Corrected

1. **Unsigned control word at signed Windows API parameter.** The active IBKR nonblocking call now explicitly preserves the FIONBIO word. This fixes the reported compile warning without disabling warnings, changing the command, or disabling socket setup. An LLVM Windows-ABI fixture reproduces the warning before and passes after; it is not a full Windows SDK build. A real local native-I/O regression is registered on Windows and Linux; only Linux execution has been performed here.
2. **Append after an externally changed historical event.** The financial operations repository previously checked only the high-water revision before a new append. The reproduction edits an older funding event but leaves the revision unchanged. Original code accepted a new command while its loaded balance differed from replay. Prefix comparison now refuses that append with no receipt or new event. External data is not repaired or cryptographically authenticated.
3. **UI/command policy drift.** Close diagnostics and execution now call one evaluator. Existing fail-fast policy and lifecycle controls remain. The original predicate is retained for review.

## Remaining boundaries

- Actor identifiers in this local simulation are not authenticated identities. This batch does not enable live orders, banking or external payments.
- A review is a snapshot of the loaded service, not an exclusive lock or signed approval. Refresh and reload are distinct operations. Apply revalidates controls and stored history before a new append.
- Each operations handle is single-thread-owned. Shutdown must wait for its command queue; do not destroy a service while another caller is reading it. Snapshot lifetime is separately owned.
- Stored event comparison is O(number of retained events). The profile is bounded to 512 events. This is not a throughput guarantee or a replacement for durable event-store scaling.
- Historical idempotent receipts remain cache-based. They do not initiate a new append and are outside the new prefix-read path.
- Reporting checks the existing global latest-posting reconciliation policy. It does not claim to reconcile independently dated external statements for a closed past period.
- The graphical period report can contain private account/order identifiers. Existing Copy report and export actions need deliberate user sharing decisions.
- The financial adapter and full Windows runtime have not been compiled or executed in this environment. No real broker is contacted by tests. Winsock command-word ABI validation does not prove authenticated IBKR protocol compatibility.
- No new disk-writing feature is enabled. Keep physical-media writes disabled.

## Development findings retained in evidence

An initial new test-main line failed strict indentation diagnostics and was reformatted. The first future-order test omitted its required October period; the fixture was corrected rather than weakening date controls. Initial and final logs remain separate. Network toolchain downloads were unavailable; a complete Windows build is not claimed.

## Validation interpretation

Native GCC and Clang sanitizer runs include the changed repository and report paths plus retained finance and broker regressions. Allocation checks, outliving-source snapshots, stale evidence, multiple currencies, reconciliation ordering, incomplete output buffers and an ephemeral real socket are exercised. These focused results do not establish repository-wide freedom from memory leaks, races, malicious inputs or sudden-power-loss defects.

## Integration inventory retained

The new native socket test initially changed the exact expected inventory used by Batch 36's composition regression. The expected-name list was updated to include that test, not relaxed to ignore differences. An intermediate incorrect name was corrected against the actual CTest registration. The complete final integration host passed 198/198 cases; earlier failing and interrupted logs are retained separately. The earlier Bank Review host also passed its unchanged 128/128 cases.
