# Banking command review and stored-history integrity

The canonical `Umicom::bank_operations` service remains the only banking state owner. The new public interface is `umicom/bank_operations/review.h`. No separate ledger, database handle or actor-authentication service is introduced.

## Calls and ownership

`UmiBankOperationsReview` validates via private `BankPrepare`, using the same `BankApply` transition as direct execution. It produces an owned immutable `UmiBankReview`; no database operation occurs. Destroy it explicitly. It owns a snapshot and copy of the canonical audit prefix and is independent of the originating handle's lifetime. Serialise every use of an operations handle, including reads and capture. It is not a concurrent snapshot API.

`UmiBankReviewSnapshotRead` returns copied prediction fields. `before`/`after` include counts and revisions; account rows identify changes in balances or debit eligibility; resolved transfer fields retain the amount, participants and state even for approval-only actions; journal/reconciliation fields are candidates. Unchanged money does not mean an operation has no business effect. Rejection, approval, customer state and card-state transitions are explained by their command and counts.

`UmiBankReviewDescribe` emits bounded plain text, using canonical money formatting. Measure with NULL/0; the returned required size includes NUL. An insufficient buffer is cleared, not presented as a complete financial report. The supplied 65536-byte UI buffer accommodates the bounded current profile; failure remains explicit.

`UmiBankReviewMatches` compares current form fields, expected revision, actor identity and capability flags against canonical event encoding. Structure padding is not part of equality. `UmiBankOperationsExecuteReviewed` checks that same authority input and exact cached event history, then calls `UmiBankOperationsExecute`. It does not bypass the existing permission, maker/checker, amount, availability, lifecycle or idempotency checks.

## Candidate and commit boundary

`BankPrepare` is the shared candidate-construction helper. The old direct Execute implementation is preserved in a disabled block for engineering comparison. The replacement executes exactly one candidate transition and publishes it only after the repository succeeds. All failed execution receipts remain zero.

For every new command, the repository starts a Data Server transaction, reads the high-water revision, verifies the event-key inventory, and compares the encoded existing events with the cached prefix. It appends the new event and high-water mark inside that transaction. Missing/invalid inventory is PARSE_ERROR, changed prefix bytes are BUSY, and Data Server failures are propagated. A rollback failure poisons the handle as before.

No schema or UBANK1 event-codec migration is required. Canonical history is compared byte-for-byte, not authenticated cryptographically. A database owner can replace a valid history; this mechanism detects disagreement with an already-loaded service, not authenticity of a newly opened database. The historical idempotent fast path remains cache-based and is not an independent database revalidation.

## Revision, identity and equivalent handles

A review is bound to history, not a filesystem path. Equivalent event histories on another handle can apply it. Changed local revision/history or changed stored history refuse a new command. Reapplying the same review after a successful new commit is BUSY. Obtain a fresh review to inspect an existing idempotent receipt. Identical requests must preserve actor and canonical payload; a conflicting request ID remains ALREADY_EXISTS.

The caller supplies identity/capabilities. The existing test-role GUI is a simulation, not authentication. Removing a capability or changing role invalidates the review, but a hostile caller able to forge the actor structure is outside this boundary. Do not expose this API directly to an untrusted network client.

## UI integration

The existing Framework GTK adapter adds Review command and Command review. All original pages and controls remain. Editing any form field or identity invalidates the review; Submit checks again. Reload/New request invalidate it. The adapter owns and clears its review on destruction, including the retained-widget interval before finalisation. These GTK paths and the six new lifecycle test cases were supplied for target-platform validation; they were not compiled or run in the delivery host.

## Limits and performance

Existing capacities remain 64 records per catalogue, 256 accepted events, and nonnegative scale-0..9 money. Each review temporarily holds a candidate plus copied snapshot/history. Each new commit now performs a bounded prefix read; this is an intentional integrity cost, not a throughput claim. No history pruning, database migration, automatic refresh, online banking, product/interest engine or authentication implementation is included.

## Integration

`UmicomBankOperations.cmake` includes `UmicomBankReview.cmake`; tests use absolute source paths and `include`, avoiding late/deferred subdirectory creation. The focused example compiles canonical finance and Data Server subsets. Its package `UmicomBankReview` is an isolated teaching SDK; the production owner remains `Umicom::bank_operations` in the full Framework SDK.
