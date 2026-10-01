/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/activity.h
 * PURPOSE: Capture business-date filtered account activity and its complete balance context.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_OPERATIONS_ACTIVITY_H
#define UMICOM_BANK_OPERATIONS_ACTIVITY_H
#include "umicom/bank_operations/operations.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiBankActivityDirection {
    UMI_BANK_ACTIVITY_ALL = 0, UMI_BANK_ACTIVITY_DEBITS = 1, UMI_BANK_ACTIVITY_CREDITS = 2
} UmiBankActivityDirection;
/** Account is required. Each all-zero date is unbounded; other dates must be
 * valid Gregorian dates in 1600..9999. Both bounds are inclusive. Reference
 * is an optional case-sensitive literal substring of referenceId or journalId,
 * with the same ASCII letters/digits/dot/dash/underscore alphabet as bank IDs.
 * A zero-initialised query selects every posting for its assigned account. */
typedef struct UmiBankActivityQuery {
    UmiFinancialId accountId;
    UmiFinancialDate fromDate, toDate;
    UmiBankActivityDirection direction;
    char reference[UMI_FINANCE_ID_CAPACITY];
} UmiBankActivityQuery;
typedef struct UmiBankActivitySummary {
    UmiBankActivityQuery query;
    UmiBankBalance balance; /* Complete unfiltered balance at captured revision. */
    uint64_t revision;
    size_t totalPostings, count;
    int64_t debitMinor, creditMinor, netMinor; /* Matching rows only. */
    bool durable;
} UmiBankActivitySummary;
typedef struct UmiBankActivityRow {
    UmiBankStatementLine posting; /* Original revision order and booked balance. */
    UmiFinancialId actorId;
    UmiBankAction action;
    bool reversal;
} UmiBankActivityRow;
typedef struct UmiBankActivity UmiBankActivity;
/** Parse exactly YYYY-MM-DD or an empty string for an unbounded date.
 * Failure leaves output unchanged; whitespace/partial dates are not accepted. */
UmiStatus UmiBankActivityDateParse(const char *text, UmiFinancialDate *outDate);
UmiStatus UmiBankActivityQueryValidate(const UmiBankActivityQuery *query);
const char *UmiBankActivityDirectionName(UmiBankActivityDirection direction);
/** Capture on the service's serial owner thread. No reload, posting, approval
 * or storage write occurs. Rows remain in posting revision order, even with
 * backdated business dates. Row balances include all prior postings, not only
 * matches. Filter totals are not opening/closing or available balances.
 * Total debit/credit overflow returns CAPACITY_EXCEEDED without a partial
 * report. Failure clears *outActivity. A valid empty result is distinct from
 * an unknown account (NOT_FOUND). The owned report survives service teardown. */
UmiStatus UmiBankActivityCapture(const UmiBankOperations *operations,
    const UmiBankActivityQuery *query, UmiBankActivity **outActivity);
void UmiBankActivityDestroy(UmiBankActivity *activity);
/** Copied reads leave output unchanged on failure. */
UmiStatus UmiBankActivityReadSummary(const UmiBankActivity *activity, UmiBankActivitySummary *outSummary);
UmiStatus UmiBankActivityRowAt(const UmiBankActivity *activity, size_t index, UmiBankActivityRow *outRow);
/** Describe/export the exact captured report, never recapture newer state.
 * Text measurement accepts NULL/0; required includes NUL. A short buffer is
 * cleared, returns CAPACITY_EXCEEDED and reports required size. Other failures
 * clear output and set required=0. Output must not alias inputs or outRequired. */
UmiStatus UmiBankActivityDescribe(const UmiBankActivity *activity,
    char *output, size_t capacity, size_t *outRequired);
UmiStatus UmiBankActivityExportCsv(const UmiBankActivity *activity, UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
