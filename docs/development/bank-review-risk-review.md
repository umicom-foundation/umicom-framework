# Batch 37 — Focused risk review

## Reproduced defect: revision-only append validation

The baseline BankRepositoryCommit checked its persisted high-water revision but not the existing event prefix. In a disposable SQLite test, an out-of-band edit changed the recorded test credit from 100000 to 200000 minor units without changing revision 6. A new transfer was then submitted through the already-open banking service.

The original implementation returned success at revision 7 while reporting 100000 booked in its cache; reopening the same database reconstructed 200000. The replacement returned BUSY with a zero receipt and retained revision 6. It did not undo or conceal the external edit. Source, build and result records are in the separate evidence archive under reproduction/.

Every new commit now verifies the canonical event inventory and exact known prefix within the same Data Server transaction used for appending its event and high-water mark. Missing/extra event keys and changed content cannot silently become the basis of a new accepted command. The stored UBANK1 encoding and existing bounded capacities remain unchanged.

## New review behaviour

The review prepares a disposable candidate through the same private BankPrepare/BankApply path used by ordinary commands. It owns its copied prediction and event history. Applying it checks actor identity/capabilities, exact form fields, loaded history and the final repository state. It never owns a second ledger or persists a guessed prediction.

The previous direct-execution implementation and GUI Submit handler remain in disabled blocks with explanations. Native failure tests check null inputs, allocation failure, stale state, altered identity, exact request reuse, wrong currency, insufficient funds and datastore failures. Existing banking and MoneyText regressions remain registered and pass in the focused tested builds.

## Residual risks and explicit limits

| Boundary | Remaining limitation |
|---|---|
| Authentication | Actor IDs and capabilities are trusted caller inputs. Test roles in the current GUI are not credentials, authenticated identities or a defence against a caller that can forge those fields. |
| Permissions | A review is not a maker/checker approval. Approval remains a separate canonical command. Same-history handles can apply a review; it is not bound to a pathname or signed identity. |
| Local history | Review capture reads cached state; it does not reload. Reload and inspect before reviewing a database another process may have changed. |
| Historical receipts | The existing idempotent fast path remains cache-based. Its receipt is not independent evidence of the present database or external settlement. |
| Database authenticity | Exact-prefix comparison detects disagreement with loaded history. It cannot prove authenticity after an administrator rewrites a complete valid history. There are no signatures or protected external audit records. |
| Recovery | A refusal retains evidence and previous cached state. No automatic repair, rollback of external edits, archive pruning or data migration is provided. |
| Concurrency | Bank Operations handles require serial access, including capture and reads. Data Server transaction locking does not make the higher-level banking handle thread-safe. Stop all calls before destruction. |
| Resource use | Reviews copy bounded state/history; commits read up to 256 prior events. This adds integrity cost. There is no throughput, multi-process scalability or latency guarantee. |
| Financial scope | Current simulation uses integer minor units and existing scale rules; no real payment network, regulated banking readiness, new fee/interest/product model or multi-currency conversion is supplied. |
| Sensitive memory | Review text and copied histories can include names, IDs and amounts. They are plaintext and are not securely wiped on free. Do not share personal output or place it in source control. |
| GTK/Windows | The real adapter source and six new white-box helper/lifecycle tests are supplied but not compiled/run in this environment. Target-platform widget lifetimes, keyboard flow, DPI, installed startup and Windows SQLite behaviour remain unverified. |
| Old delivery gaps | Physical media writing, authenticated IBKR sessions and complete operating-system boot qualification remain open. This banking update does not establish any of those results. |

## Evidence boundaries

Native GCC and Clang sanitizer runs test the actual canonical banking, finance and Data Server implementations through a focused dependency subset. They do not certify the complete repository or prove all application paths leak-free. Static analysis reported no diagnostics for four affected native files; the GTK adapter was not in that analysis.

SQLite-disabled cases are reported as skipped. The retained Batch 36 integration suite was run separately and does not mean the new banking review was injected into each of its child compositions. Its two inherited socket tests use an inert local peer, never an authenticated broker.

Earlier authoring attempts are retained where relevant: a test helper initially used a nonexistent remove function and was corrected to the actual Data Server delete API before final native runs. The first retained-integration attempt omitted its quotes.csv fixture when assembling the private dependency subset; the fixture was restored unchanged and all 197 cases then passed. The shell deadline also interrupted that initial attempt. Neither issue was resolved by suppressing a test or modifying unrelated production behaviour.
