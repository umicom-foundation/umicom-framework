# Build a reviewed practice-interest workflow

Framework owns banking commands, approval rules, the event log and journal.
A product supplies forms and trusted actor information. Link `Umicom::bank_operations`
and use `umicom/bank_operations/operations.h`, `review.h` and `statement_text.h`.
The existing local banking simulation is the supported execution profile.

## Calculate an amount without posting it

```c
UmiBankingInterestAccrual calculation;
int64_t interest_minor = 0;
UmiStatus status = umi_banking_interest_accrual_init(
    &calculation, "example", 100000, 500, 30, 365);
if (status == UMI_STATUS_OK)
    status = umi_banking_interest_accrual_calculate(&calculation, &interest_minor);
/* Use interest_minor only after checking status. Expected result: 410. */
```

Include `umicom/finance/banking/interest_accrual.h` for this example and link
`Umicom::finance`. The checked call uses integers, truncates toward zero once,
and reports an unrepresentable result. It leaves a separate output unchanged
on failure. The older value-only call is still available; it returns zero on
error, so use the checked call when zero and failure must be distinguished.
The calculator supports signed rates; the posting workflow accepts positive
rates from 1 through 10,000 basis points.

## Submit through the existing service

1. Open a memory or SQLite banking service. Keep each handle on one serial
   queue, including all reads and reviews. Create a customer, account and
   practice opening credit using existing commands.
2. Initialise a command with `UmiBankCommandInit`. For `UMI_BANK_INTEREST_SUBMIT`,
   set `id` to a new interest ID, `ownerId` to your period ID and `sourceAccountId`
   to the receiving account. Set `interest` to `{500, 30, 365}`. Leave the money,
   name, destination and state fields zero. Supply request ID, business date,
   timestamp and the observed revision as with other banking commands.
3. Call `UmiBankOperationsReview`. Read the copied review snapshot or describe it
   with `UmiBankReviewDescribe`. It includes the fixed booked principal and the
   calculated amount. No event or balance is published by this review.
4. After the user accepts that exact review, call
   `UmiBankOperationsExecuteReviewed` with the same actor. Destroy the owned
   review after use. Edits, identity changes and stale history require a new review.
5. A different checker uses `UMI_BANK_INTEREST_APPROVE`; an operator uses
   `UMI_BANK_INTEREST_POST`. These reference the interest ID and leave interest
   terms zero. Approval preserves the maker's calculation; posting adds a
   balanced journal through the existing Data Server transaction.
6. Read requests with `UmiBankOperationsInterestAt` and the count in
   `UmiBankCounts.interestRequests`. Use reject/cancel for unposted requests and
   `UMI_BANK_INTEREST_REVERSE` for a posted amount. Reversal respects available
   funds and appends a journal instead of deleting the original.

The stored event retains submission terms, and replay derives the same principal
from its preceding journals. The period ID is unique per account while a request
is pending, approved or posted. It is an explicit label, not a calendar interval;
the application must choose consistent labels. The interest amount uses a fixed
principal, not historical daily balances or automatic compounding. Request IDs
still protect retry identity; a period ID prevents a second active request for
that labelled period even when it has a different entity ID.

## Describe a statement

Call `UmiBankOperationsDescribeStatement` with an account and inclusive first and
last revisions. A `NULL` output and zero capacity measures the required size,
including its terminator. Allocate that size and call again on the same serial
queue. The convenience `UMI_BANK_STATEMENT_TEXT_CAPACITY` covers the bounded
profile. A short buffer is reported and cleared rather than returned as a
complete report. Always check the result.

Earlier journals determine the opening balance. Entries in the requested range
include original postings and later reversals separately. Holds and unposted
approvals have no journal entry. The report names the cached service revision;
reload explicitly before reporting newer external commits. This call does not
save a file, change a balance or approve a command.

## Rebuild and verify

Public command, count and review structures gained fields. Recompile Framework
and all clients together; initialise commands with the provided initializer.
This source update does not promise binary compatibility with prebuilt clients.
Historical event actions retain their encoded bytes. Only the new interest
submission action appends its three terms, and old executables reject the new
actions. Keep your pre-upgrade database copy if you need to return to an old build.

The tests under `tests/bank_interest` exercise arithmetic, permissions, journals,
restart, stale writers, rollback, reports and native forms. After configuring and
building the Applications all-module preset, run:

```powershell
& "C:\msys64\ucrt64\bin\ctest.exe" --preset windows-ucrt64-all-debug -R 'framework\.bank_interest\.|bank\.operations\.interest_lesson|bank\.module\.layout_library' --parallel 2 --no-tests=error --output-on-failure
```

SQLite or GTK skips do not verify those integrations. Use a build with the required
backend and a desktop display for the corresponding acceptance checks.
