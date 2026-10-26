/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/accounting_review.h
 *
 * PURPOSE:
 *   Bind local accounting approval and posting previews to an immutable complete batch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_OPEN_TMS_ACCOUNTING_REVIEW_H
#define UMICOM_OPEN_TMS_ACCOUNTING_REVIEW_H
#include "umicom/open_tms/accounting_book.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiOpenTmsAccountingAction {
    UMI_OPEN_TMS_ACCOUNTING_APPROVE = 0,
    UMI_OPEN_TMS_ACCOUNTING_POST
} UmiOpenTmsAccountingAction;
typedef struct UmiOpenTmsAccountingReview UmiOpenTmsAccountingReview;
typedef struct UmiOpenTmsAccountingReviewSnapshot {
    UmiOpenTmsAccountingAction action;
    uint64_t source_revision, next_revision;
    size_t line_count;
    int approved_before, posted_before, approved_after, posted_after;
} UmiOpenTmsAccountingReviewSnapshot;
/* Copy the entire active batch and preflight one existing local action.
 * The source may be released or changed afterwards. Creation performs no
 * mutation, I/O, callbacks or ledger posting. Failure clears out.
 * The compatibility book retains its documented double/tolerance semantics;
 * this review is not authenticated approval or exact-money posting authority. */
UmiStatus umi_open_tms_accounting_review_create(const UmiOpenTmsAccountingBook *book,
    UmiOpenTmsAccountingAction action, UmiOpenTmsAccountingReview **out);
void umi_open_tms_accounting_review_destroy(UmiOpenTmsAccountingReview *review);
/* Copy accepted preview metadata or one complete line in original order.
 * Out-of-range line returns NOT_FOUND; refusal leaves outputs unchanged. */
UmiStatus umi_open_tms_accounting_review_read(const UmiOpenTmsAccountingReview *review,
    UmiOpenTmsAccountingReviewSnapshot *out);
UmiStatus umi_open_tms_accounting_review_line(const UmiOpenTmsAccountingReview *review,
    size_t index, UmiOpenTmsAccountingLine *out);
/* Recheck source revision, flags and every active line value before applying.
 * Do not compare padding or inactive array tails. Even unversioned field
 * changes invalidate the preview. Refusal preserves the entire book.
 * Success advances once, so replay refuses. Only flags/revision change;
 * existing lines and unused storage remain untouched.
 * Serialize owner access; outputs must not overlap inputs or the live book. */
UmiStatus umi_open_tms_accounting_review_apply(UmiOpenTmsAccountingBook *book,
    const UmiOpenTmsAccountingReview *review);
#ifdef __cplusplus
}
#endif
#endif
