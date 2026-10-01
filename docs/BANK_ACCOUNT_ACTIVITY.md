# Capture an immutable account activity report

Account activity is a filtered view of the canonical banking statement. It
uses the existing journal and balance services. No application needs to build
a second ledger to add date or reference filters.

## Use the report service

1. Include **umicom/bank_operations/activity.h**.
2. Zero-initialise **UmiBankActivityQuery** and assign its exact account ID.
   Zero dates leave either end unbounded; direction zero includes all postings.
3. Use **UmiBankActivityDateParse** for exact **YYYY-MM-DD** input. It accepts
   an empty string for an unbounded date and validates Gregorian dates using
   the shared financial date contract.
4. Optionally select debits or credits and set a literal, case-sensitive
   substring of the reference or journal ID.
5. Call **UmiBankActivityCapture** on the banking service's serial owner thread.
   Capture reads the current cached state. It does not reload or write storage.
6. Use copied summary and row reads, **UmiBankActivityDescribe**, or
   **UmiBankActivityExportCsv**. All of them refer to the same captured evidence.
7. Destroy the report with **UmiBankActivityDestroy**. Destroy an exported CSV
   separately with **UmiCsvDocumentDestroy**.

~~~c
UmiBankActivityQuery query = {0};
UmiStatus status = umi_financial_id_assign(&query.accountId, "account");
if (status == UMI_STATUS_OK)
    status = UmiBankActivityDateParse("2026-10-01", &query.fromDate);
if (status == UMI_STATUS_OK)
    status = UmiBankActivityDateParse("2026-10-31", &query.toDate);
UmiBankActivity *activity = NULL;
if (status == UMI_STATUS_OK)
    status = UmiBankActivityCapture(operations, &query, &activity);
if (status == UMI_STATUS_OK) {
    UmiBankActivitySummary summary;
    status = UmiBankActivityReadSummary(activity, &summary);
    /* summary.balance is complete; summary.debitMinor and creditMinor
     * include only rows matching the query. Check status before use. */
}
UmiBankActivityDestroy(activity);
~~~

## Preserve the meaning of money

The summary's complete balance uses the canonical booked, reserved and available
values at the captured revision. Filters do not change those values.

Debit, credit and net totals include matching postings only. All amounts use
the same account currency and scale, in integer minor units. Addition checks
for signed 64-bit overflow before publishing a report. If a turnover total
would overflow, capture returns **CAPACITY_EXCEEDED** and no report. A narrower
query may still be representable.

Rows retain their original posting revision order and the canonical booked
balance after each posting. A business-date filter does not reorder backdated
entries and does not make a filtered running balance. Reversals are separate
rows with the compensating flag. Holds and unposted requests are absent from
the posting list but may contribute to the complete reserved balance.

Capture also copies each matching posting's actor and action from its original
event and its reversal flag from the journal. Revision and reference checks
ensure that the projection does not attach unrelated evidence.

## Ownership, failures and storage

The report owns its copied query, summary and rows. It can outlive the source
service. Changing a caller's query or committing another operation cannot
change that report. There is no mutable report-row pointer in the public API.

Capture failures clear the output report pointer. Unknown accounts return
**NOT_FOUND**; existing accounts with no matching postings return a valid
empty report. Copied reads leave caller outputs unchanged on failure.
The date parser also leaves its output unchanged on invalid input.

Text supports a **NULL, 0** size query; the reported size includes the terminating
NUL byte. An undersized buffer is cleared and returns **CAPACITY_EXCEEDED**.
CSV exports the exact capture with explicit filter and balance columns.
Summary amounts are not repeated as totals on each posting row.

SQLite handles retain cached state until an explicit reload. Reports identify
their captured revision and whether the source used durable local storage.
The report itself is a memory object, not a separately saved report archive.
No posting, reconciliation or identity capability is granted by reading it.

## Native integration

Bank appends an **Account activity** page to its existing operation window.
Filter edits discard the old capture and disable copying. Capture publishes
text only after the whole report is ready. Other ledger changes leave an
already displayed capture readable with its original revision. Close destroys
the report; retained controls cannot access the closed banking service.

The thin Bank facade is **UmiBankCaptureActivity**. Its complete memory-only
lesson is in the Bank module at **src/console/activity_example.c**. Regression
sources under **tests/bank_activity** cover date parsing, backdated postings,
exact totals, empty results, overflow, immutable exports, SQLite reload and
native control lifetimes. Execute them on the target platform before making
runtime or release claims.
