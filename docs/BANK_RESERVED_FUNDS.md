# Explain reserved account funds

An account can have money booked in its ledger that is not currently available
for another debit. A reservation sets that money aside. Framework's reserved
funds report explains the difference using the existing local-practice ledger.

The relationship is **available = booked - reserved**. Three categories explain
the reserved amount: active manual holds, active card authorisations, and
outgoing transfers waiting for approval or execution. An approved transfer still
reserves funds until it is executed or cancelled. Incoming transfers do not
reserve the destination account's funds. Interest and charge requests reserve
nothing; they remain visible in the separate review queue.

## Read an account's explanation

1. Open Bank's banking operations window and select **Reserved funds**.
2. Enter an exact account ID, then choose **Capture reserved funds**. If another
   window changed the database, use **Reload committed data** first.
3. Read the captured revision and storage description, then the booked,
   reserved and available amounts. The category totals must add to reserved.
4. Inspect the records in creation-revision order. Each has a type, ID,
   exact amount, creator and creation revision. Card records identify their
   card; transfers identify the destination and beneficiary.
5. Choose **Copy captured reservations CSV** to copy exactly that report.
   It does not silently reload the account or recompute a newer report.

For example, a booked GBP 1,000.00 balance with a GBP 10.00 manual hold,
GBP 20.00 card authorisation and GBP 30.00 pending transfer has GBP 60.00
reserved and GBP 940.00 available. These records are not posted debits.
A final card capture posts its captured amount and releases any unused
authorisation. Its old reservation then disappears from a fresh report;
the posted debit remains in the [account activity](BANK_ACCOUNT_ACTIVITY.md).

## Review a release or cancellation

1. Choose an appropriate trusted test identity in the existing command form.
   Test operator can release manual holds, void card authorisations and cancel
   transfers. A transfer's original maker can cancel their own transfer.
2. Select a captured reservation explicitly. A fresh capture selects no row.
3. Choose **Review manual hold release**, **Review card authorisation void**,
   or **Review transfer cancellation**, according to the selected record.
4. Read **Command review**, including the resolved amount, account, lifecycle
   state and predicted balance change. No command has been committed yet.
5. Use the existing **Submit** control only after deciding to apply that review.
   The booked balance stays unchanged for these release/cancellation actions;
   reserved falls and available rises.
6. Capture again to see the new reservation list. The old report deliberately
   remains an earlier capture, even after successful submission.

Changing the selected row, account, command fields or identity invalidates the
prepared review. A stale capture or different event history is rejected.
If a second window commits after review, execution also rejects the stale
database revision. Reload, capture and review again; never automatically retry.
A blocked account can still have an existing reservation. Canonical release
rules, rather than the report, decide whether the requested action is allowed.

## Use the C API

1. Capture with `UmiBankReservationsCapture` on the operations owner's serial
   thread. A known account with no reservations yields a valid empty report;
   an unknown account returns NOT_FOUND. Failure clears the report pointer.
2. Read copied rows and summary values. Their totals are checked against
   `UmiBankOperationsBalance`, which remains the balance authority. Up to
   64 holds and 64 transfers can contribute. Currency and scale must match.
3. Resolve the selected row's release action with
   `UmiBankReservationsReleaseAction`. Kind plus ID identifies the row; a hold
   and a transfer may legally have the same ID.
4. Construct a new command with that action, ID, request ID, business date,
   timestamp and captured revision. Call `UmiBankReservationsReviewRelease`.
   It compares complete canonical event history, not merely the revision number,
   then delegates capabilities and economics to the existing command review.
5. After a separate decision, call `UmiBankOperationsExecuteReviewed`. Destroy
   the review and report when finished. Neither borrows the operations handle.

The shared review now includes copied hold before/after values for placement,
authorisation, release, void, capture and refund. A partial final capture keeps
the original authorised amount separately from the captured amount. The review
snapshot grew; rebuild Framework and all consumers together. The event format,
action numbers, ledger ownership and persistence format are unchanged.

## Know the limits

This is local practice evidence, not a live bank authorisation service. There is
no automatic hold expiry, partial manual release, multi-capture card settlement,
batch release, remote network request or external account reconciliation here.
Capabilities must come from trusted host code; the test identity selector is not
authentication. The report does not change account permissions or bypass the
existing maker/approver separation for transfers.

Text and CSV use exact integer money. CSV includes currency, scale, captured
revision and a local-practice label; complete balances appear once in the summary
row to avoid accidental repeated totals. Reports remain readable after service
teardown but are not themselves persisted. Clipboard retention after exit is
platform-dependent. A later capture reads cached committed state unless the
owner explicitly reloads it.
