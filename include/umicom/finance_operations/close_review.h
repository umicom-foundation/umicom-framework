/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance_operations/close_review.h
 * PURPOSE: Explain period-close controls from owned, read-only ledger snapshots.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Explain the existing period-close controls using an owned, read-only snapshot.
 * Amounts reuse canonical trial balances; no second ledger or close policy.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_OPERATIONS_CLOSE_REVIEW_H
#define UMICOM_FINANCE_OPERATIONS_CLOSE_REVIEW_H
#include "umicom/finance_operations/operations.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFinanceCloseReview UmiFinanceCloseReview;
typedef enum UmiFinanceCloseIssueKind {
    UMI_FINANCE_CLOSE_EARLIER_PERIOD = 1,
    UMI_FINANCE_CLOSE_UNPOSTED_JOURNAL,
    UMI_FINANCE_CLOSE_OPEN_ORDER,
    UMI_FINANCE_CLOSE_UNSETTLED_FILL,
    UMI_FINANCE_CLOSE_MISSING_RECONCILIATION,
    UMI_FINANCE_CLOSE_MISMATCHED_RECONCILIATION,
    UMI_FINANCE_CLOSE_STALE_RECONCILIATION,
    UMI_FINANCE_CLOSE_TRIAL_BALANCE_UNAVAILABLE,
    UMI_FINANCE_CLOSE_UNBALANCED_TRIAL_BALANCE
} UmiFinanceCloseIssueKind;

typedef struct UmiFinanceCloseIssue {
    UmiFinanceCloseIssueKind kind;
    UmiFinancialId entityId;
    UmiFinancialId evidenceId; /* Empty where there is no reconciliation. */
    uint64_t requiredPostingRevision;
    uint64_t observedPostingRevision;
    UmiStatus status;
} UmiFinanceCloseIssue;

typedef struct UmiFinanceCloseReviewInfo {
    UmiFinanceOperationPeriod period;
    uint64_t revision;
    size_t issueCount;
    size_t currencyCount;
    UmiStatus evaluationStatus;
    bool checksPass;
    bool writesBlocked;
    bool canPrepare;
    bool canFinalise; /* Actor separation must still be checked during Apply. */
    bool durable;
} UmiFinanceCloseReviewInfo;

/** Capture from the single-thread-owned operations service. No Data Server
 * transaction, reload or mutation is performed. The snapshot describes the
 * loaded revision, not proof that another writer has not since changed it.
 * Its queries remain valid after the service and Data Server are destroyed.
 * A report with blockers is a successful capture, not a successful close.
 * On failure *out is NULL. */
UmiStatus UmiFinanceCloseReviewCreate(const UmiFinanceOperations *operations,
    const char *periodId, UmiFinanceCloseReview **out);
void UmiFinanceCloseReviewDestroy(UmiFinanceCloseReview *review);
/** Outputs are copies; on error the caller's record remains unchanged. */
UmiStatus UmiFinanceCloseReviewGetInfo(const UmiFinanceCloseReview *review,
    UmiFinanceCloseReviewInfo *out);
UmiStatus UmiFinanceCloseReviewIssueAt(const UmiFinanceCloseReview *review,
    size_t index, UmiFinanceCloseIssue *out);
UmiStatus UmiFinanceCloseReviewTrialBalanceAt(const UmiFinanceCloseReview *review,
    size_t index, UmiFinanceTrialBalance *out);
const char *UmiFinanceCloseIssueText(UmiFinanceCloseIssueKind kind);
/** Format a complete plain-text report. *required includes the NUL terminator.
 * A NULL buffer with zero capacity measures the report and returns
 * CAPACITY_EXCEEDED. Insufficient caller storage is left as an empty string.
 * The text can contain private account and order identifiers. */
UmiStatus UmiFinanceCloseReviewFormat(const UmiFinanceCloseReview *review,
    char *buffer, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
