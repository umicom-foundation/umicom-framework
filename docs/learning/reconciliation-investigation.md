# Model reconciliation investigation without changing the ledger

A comparison records what the ledger and an entered external balance said at
one point in time. Investigation adds a disposition to an unmatched comparison.
Keeping these separate means an investigated difference never becomes a
fictional matching original observation.

## Follow the shared workflow

1. Record comparisons with the existing **UMI_BANK_RECONCILE** action.
2. Query a copied record with **UmiBankOperationsFindReconciliation** or enumerate
   records with **UmiBankOperationsReconciliationAt**.
3. Create a resolve command with **UMI_BANK_RECONCILIATION_RESOLVE**. Its **id**
   identifies the unmatched comparison; **ownerId** identifies fresh matching
   evidence; **name** explains the investigation.
4. Set a unique request ID, business date, timestamp and the current expected
   revision using the same command envelope as other banking actions.
5. Call **UmiBankOperationsReview** with the operator actor. Present the owned
   review using **UmiBankReviewDescribe**. The copied before/after reconciliation
   and linked evidence remain readable independently of the service lifetime.
6. On explicit confirmation call **UmiBankOperationsExecuteReviewed**. Check its
   status and receipt. Destroy the review when the interaction is finished.

A successful new disposition creates one event and advances the committed
revision. It creates no account, comparison, hold or journal. **matched**, the
original revision, and both original money values remain unchanged.

## Understand evidence freshness

Evidence must be a matching comparison for the same account and the latest
recorded comparison for it. It must have been recorded after the original break
and any later disposition. No journal may affect that account after the
evidence. The current booked money must still equal its captured booked money.

The rule uses committed revision order. User-entered business dates and
timestamps do not establish freshness. Even offsetting account postings make
old evidence stale. A reservation alone changes available funds, not the booked
balance being compared. Other accounts' events can invalidate an already-open
review through its history guard, but do not prevent preparing a new resolution
against otherwise fresh evidence.

## Reopen and retain history

Use **UMI_BANK_RECONCILIATION_REOPEN** with the resolved break's ID and a new
explanation. Leave unused payload fields zero. Reopening keeps the previous
evidence ID and updates the latest reviewer, reason and review revision.
Every earlier resolve/reopen command remains in the event log. Later account
activity does not automatically reopen an already resolved historical break.

Fresh matching evidence after the reopening is required for another resolve.
Calling an old resolve request again returns its existing idempotent receipt;
it does not undo the newer reopening. A reused request ID with different
canonical fields is rejected.

## Keep ownership and authority clear

Both actions require **UMI_BANK_CAP_OPERATE**. The application supplies trusted
actor capabilities; Framework does not authenticate a person. There is no
maker/checker separation for these non-monetary administrative actions.
Closed or blocked accounts can still be investigated.

Use one serial command queue per service. The review binds exact command,
actor and canonical history. Execution rechecks the repository inside the
existing transaction. A failed write leaves state unchanged; a newer writer
returns BUSY. Reload, inspect and prepare a new review deliberately.

The GTK adapter uses the same form metadata, review and submit path. It adds
no second case repository or financial calculator. **UmiBankReconciliationStateName**
provides stable English state labels for other adapters.

## Compatibility and limits

Old action numbers and event encodings remain unchanged. Resolve and reopen
append action numbers 35 and 36. Older readers reject these actions. The
reconciliation and review value structures grew; rebuild all consumers.

Old unmatched comparisons replay with an unreviewed disposition. Original
matched comparisons are evidence and do not enter the break lifecycle.
The 64-record and 256-event practice limits remain. External balance evidence
is entered by the caller; no file import, bank-feed verification, attachments,
case assignment or automatic ledger repair is supplied.
