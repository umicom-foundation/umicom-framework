# Build a reviewed practice charge workflow

An account charge is a fixed debit from a practice account. Framework owns the
rules and records. An application supplies actor identity, command fields and
the user's review step; it must not subtract a separate balance in its widgets.

Include `umicom/bank_operations/operations.h` and
`umicom/bank_operations/review.h`, and link `Umicom::bank_operations`. Applications
must rebuild against the new headers because the counts and review snapshot
structures have additional charge fields. Existing action numbers remain stable;
the six charge actions are appended as values 29 through 34.

## Follow one charge through the account book

1. Open a memory or SQLite operations owner with the existing Bank operations
   API. Create a customer and an account and supply practice funding using their
   normal commands. No charge action opens an account or imports money.
2. Initialise a fresh `UmiBankCommand` with `UMI_BANK_CHARGE_SUBMIT`. Set `id` to
   the new charge ID, `ownerId` to your reference, `sourceAccountId` to the account,
   `name` to a readable reason, and `amount` to a positive `UmiMoney`. GBP scale 2
   and 250 minor units means GBP 2.50. IDs follow the ordinary Bank identifier
   rules. Leave destination, record state and interest terms empty.
3. For every command, supply a distinct `requestId`, the current `expectedRevision`,
   a valid business date and a nonnegative timestamp. The submitting actor needs
   `UMI_BANK_CAP_PAYMENTS`. Use `UmiBankOperationsReview` to create an owned preview;
   `UmiBankReviewDescribe` resolves and describes the charge. Destroy that review
   with `UmiBankReviewDestroy` when it is no longer needed.
4. After the user reviews the preview, call `UmiBankOperationsExecuteReviewed`.
   Submission creates a pending request. It neither reserves nor posts money.
   Use `UmiBankOperationsChargeAt` to copy retained records; they cannot be changed
   by editing the returned copy.
5. A different actor with `UMI_BANK_CAP_APPROVE` submits `UMI_BANK_CHARGE_APPROVE`.
   Initialise another command and set only the charge `id` plus the common request,
   revision, date and timestamp fields. The review resolves the original amount,
   account and reason, so approval never uses a newly entered amount. The maker
   cannot approve or reject their own charge even with all capability bits.
6. An actor with `UMI_BANK_CAP_OPERATE` submits `UMI_BANK_CHARGE_POST`. The service
   rechecks the account and customer state and available funds. It debits the
   customer's deposit liability and credits `sys.charges.CCY` in one balanced
   journal. The charge becomes executed and keeps the posting revision.
7. To reverse the whole charge once, use `UMI_BANK_CHARGE_REVERSE` with operating
   permission. This credits the same active account, checks integer overflow,
   records the reversal revision and adds a compensating journal. The original
   posting remains queryable. Partial refunds are not part of this command.

Use the existing statement or CSV exporter to inspect both journals. Statement
revision ranges are inclusive command numbers, not dates. The charge reference
and reason are in `UmiBankChargeRequest` and the review; statement lines refer to
the charge entity ID. No floating-point conversion is needed for ledger values.

## Understand cancellation, duplicates and funds

The maker or an operator can cancel a pending or approved request. A separate
checker can reject a pending request. Neither action moves funds. Pending,
approved and executed charges must have distinct references within an account.
A rejected, cancelled or reversed reference can be corrected by submitting a new
charge entity ID. Old entity IDs and their records are retained permanently in
this bounded local profile.

Charges do not reserve funds while waiting for approval. Active holds and pending
or approved transfers still reserve their amounts and can prevent charge posting.
This avoids a second reservation system. A failed post leaves its request approved
and creates no journal. A blocked account or customer prevents posting and reversal.
Pending or approved charges prevent account closure. Closed accounts cannot reopen,
so complete any necessary reversal before permanent closure.

## Preserve state when something changes

Reviews own their captured data and may outlive the operations owner. They are
predictions from one retained revision, not permission signatures. Before execution,
Framework checks the supplied actor, current capabilities and canonical event
history. Changed fields or another committed command require another review.
SQLite writes compare and commit the canonical history transactionally; a failed
write does not publish a new in-memory balance or partially written charge.

An identical accepted request can be retried through the command API and returns
its original receipt. Reusing a request ID for different content is rejected. A
preview made before that successful command is stale and must be reviewed again;
do not treat an old preview as a reusable approval token.

Charge actions reuse the existing event fields, so older event bytes retain their
encoding. A new executable can replay old records. An old executable rejects new
charge action numbers. Replay validates the lifecycle as well as syntax; a posted
charge without its submission and approval is invalid. Do not rewrite saved events
to bypass replay failures. Memory storage never writes a database; SQLite storage
retains accepted commands across restarts.

The profile allows 64 charge requests and 256 accepted commands across the book.
Cancellation does not free retained history slots. Actor capability flags must
come from the caller's trusted policy; the Bank lesson's test-role selector is
not an authentication service. These fixed local charges do not implement tax,
percentage pricing, recurring billing or real collection providers.

For a complete product composition, read Umicom Bank's
`src/console/charges_example.c`. It opens a volatile book, reviews each step,
checks balances and prints the statement without duplicating Framework rules.
