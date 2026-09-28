# Financial close review contract

Owner: Umicom Framework. Language: C23. Licence: MIT. Author: Sammy Hegab, Umicom Foundation.

## Single authority

`Umicom::finance_operations` owns the command state. The new `FinanceEvaluateClose` replaces the active private predicate while the full previous predicate stays in `#if 0`. With no report, the evaluator preserves the original fail-fast order and status. With a report, it retains all blocking records and the same first failure. The GTK report does not duplicate those rules.

Public additions are in `umicom/finance_operations/close_review.h`. They allocate/copy a report, query its info/issues/trial balances, format a complete text report and destroy it. There are no new commands, actors, period states, schemas or persistence formats. Snapshot capture does not reload or touch Data Server. Closed periods may be inspected but neither lifecycle eligibility flag is set.

## Ownership and bounds

Capture while exclusively owning the service's serial command sequence. Captured reviews own all content and may outlive the source service. Concurrent independent reads are possible, but destruction must be sequenced after readers. No mutable pointer into the review is returned. Formatting writes into caller-owned storage, reports the required NUL-inclusive capacity, and never succeeds with truncated text.

The report has capacity for every existing blocker: periods + journals + orders + fills + two issues per account (528 slots). At most 64 separate currency/scale trial balances are retained. These are bounded heap allocations, not unbounded queues or stack-resident copies of the service. Limits inherited from financial operations remain unchanged.

Trial balances use the canonical dated posting projection; original and compensating entries are both retained. Reconciliation checks retain the current policy: latest evidence for every posted account must match its last posting revision, including postings outside the selected period. This is not a newly introduced period-only reconciliation interpretation. Currency and scale are never aggregated together indiscriminately.

## Commit-prefix integrity

`FinanceRepositoryCommit` now scans the existing event inventory and compares each persisted event with the canonical encoding of the loaded command before appending. It does this inside the existing Data Server transaction. Missing/extra keys fail; a changed existing value with the same revision returns BUSY. No event, receipt or cached state is committed in those cases. The original inventory/codec and storage operations are reused. Historical duplicate receipts are still cache-based and this addition does not authenticate persistence or reverse outside changes.

## Winsock boundary

The native IBKR adapter's `ioctlsocket` call explicitly converts `FIONBIO` to the API's signed long. Compile-time checks assert the round-trip command word and width agreement. The prior statement remains disabled for review. Nonblocking semantics, loopback restriction and read-only broker policy are unchanged. `native_nonblocking_adapter` uses its own ephemeral loopback listener, not a provider port.

## Integration

The existing `UmicomFinanceOperations.cmake` includes the new helper after defining its canonical targets. Test definitions use include with absolute paths, preserving the deferred-integration repair. Both Accountant and Exchange consume the existing GTK financial operations adapter; Accounting periods now includes the shared close text. No product module source is changed. The independent umicomOS host is a developer-tool composition, not guest banking infrastructure.

## Limitations

This is an explanatory close-control slice, not complete close orchestration or a general CCP. It does not authenticate actor labels, perform live settlement, query external statements, reserve a close, or guarantee another writer has not changed the database since capture. Final Apply remains authoritative. External history changes require investigation. No GTK, full Windows runtime or physical-media qualification is inferred from native tests.
