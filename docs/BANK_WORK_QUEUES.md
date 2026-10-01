# Capture and review open banking requests

A work queue brings pending and approved transfers, practice interest and fixed
practice charges into one list. Framework derives the rows from its existing
banking service. It does not keep another ledger or approval state machine.

The queue is a snapshot: an owned copy of the values at a particular committed
revision. It remains readable after the service closes. It does not update
automatically or reserve funds. All of these workflows use the existing local
practice profile, not a payment network or authenticated banking service.

## Capture the requests you need

1. Open a banking service and reload committed data if another handle may have
   written to its store. Capture uses the service's current cached state.
2. Include `umicom/bank_operations/work_queue.h`.
3. Start with `UmiBankWorkQueueFilterAll`. Set `kinds` to any nonzero combination
   of `UMI_BANK_WORK_TRANSFER`, `UMI_BANK_WORK_INTEREST` and
   `UMI_BANK_WORK_CHARGE`. Set `states` to pending, approved or both.
4. Leave `accountId` empty for all accounts, or assign an exact account ID.
   Transfers match either endpoint; interest and charges match their account.
   An unknown account returns `UMI_STATUS_NOT_FOUND`.
5. Call `UmiBankWorkQueueCapture`. Passing a null filter selects every open
   request. On failure, the output queue is null.
6. Read the summary and rows. `count`, `pending` and `approved` describe the
   filtered list; `totalOpen` describes every open request before filtering.

Rows are ordered by their submission revision, then kind and ID. Different
request kinds may use the same ID, so keep both kind and ID when retaining a
selection. A row index is meaningful only for the particular captured queue.

Amounts use integer minor units, currency and scale. An interest row also
contains the fixed principal and submitted terms. A charge row contains the
submitted reason and reference. Do not add amounts from different currencies
or scales to make a combined queue total.

## Prepare a review without executing it

1. Retain the captured queue while the user examines a row.
2. Use `UmiBankWorkQueueResolveAction` to map the user's decision to its existing
   banking command. Pending rows support Approve, Reject and Cancel. Approved
   rows support Post/Execute and Cancel. This mapping does not grant authority.
3. Initialise a command using `UmiBankCommandInit`. Set its ID to the selected
   row's ID and its expected revision to the queue summary's revision. Supply
   a new request ID, explicit business date and timestamp. Later workflow
   actions have no amount or other submission payload.
4. Supply the current actor from trusted application code and call
   `UmiBankWorkQueueReview`. Check its returned status.
5. Show the returned review with `UmiBankReviewDescribe`. It includes resolved
   economics and the predicted effects of the canonical transition.
6. Wait for a separate user decision before calling
   `UmiBankOperationsExecuteReviewed`. Destroy the review when it is no longer
   needed, even if the user cancels.

The review checks the row's type and ID, the action, revision and complete
canonical event history. A different database with the same revision number
does not qualify unless its history is equivalent. Actor capabilities,
maker/checker separation, account eligibility and funds remain the banking
engine's responsibility. Queue selection cannot bypass those checks.

If the service changed, expect `UMI_STATUS_BUSY`. Reload when appropriate,
capture again and let the user review the current request. Never silently swap
in a newer row, retry an approval or submit a different entity. A repository
change not yet loaded into the service may be detected later, during reviewed
execution; that execution also returns a failure without committing a candidate.

## Export or retain the evidence

`UmiBankWorkQueueExportCsv` copies the captured rows, their revision, storage
mode and applied filters. It neither recaptures current state nor writes a file.
Destroy its output with `UmiCsvDocumentDestroy`. A successful empty queue still
exports a header and summary. The shared CSV writer quotes text and guards
spreadsheet formula prefixes; amounts remain exact minor-unit values.

Destroy a queue with `UmiBankWorkQueueDestroy`. It owns its rows and history;
it does not retain the source service. Use capture, review and service mutation
on the service's serial owner thread. These APIs add no concurrent locking.

## Boundaries

The existing practice profile permits 64 records per request catalogue and
256 committed events. A queue can therefore hold up to 192 open rows; it does
not increase those service limits. Posted, reversed, cancelled and rejected
requests remain in their original tables and audit history but leave this queue.

There is no bulk approval, scheduling, notification delivery or automatic
posting. A queue review is not a reservation, signed authorisation or payment
receipt. The native Bank panel uses the same captured queue, command review and
explicit Submit action described here.
