/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/accounting_review.c
 *
 * PURPOSE:
 *   Own complete accounting review assumptions and refuse stale or replayed local actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/accounting_review.h"
#include <stdlib.h>
#include <string.h>
struct UmiOpenTmsAccountingReview {
    UmiOpenTmsAccountingBook source;
    UmiOpenTmsAccountingReviewSnapshot snapshot;
};
UmiStatus umi_open_tms_accounting_review_create(const UmiOpenTmsAccountingBook *book,
    UmiOpenTmsAccountingAction action, UmiOpenTmsAccountingReview **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (action != UMI_OPEN_TMS_ACCOUNTING_APPROVE && action != UMI_OPEN_TMS_ACCOUNTING_POST)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_accounting_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    UmiOpenTmsAccountingReview *review = calloc(1U, sizeof *review);
    UmiOpenTmsAccountingBook *candidate = malloc(sizeof *candidate);
    if (review == NULL || candidate == NULL) {
        free(candidate); free(review); return UMI_STATUS_OUT_OF_MEMORY;
    }
    review->source = *book; *candidate = *book;
    status = action == UMI_OPEN_TMS_ACCOUNTING_APPROVE ?
        umi_open_tms_accounting_book_approve(candidate) : umi_open_tms_accounting_book_post(candidate);
    if (status == UMI_STATUS_OK) {
        review->snapshot = (UmiOpenTmsAccountingReviewSnapshot){
            action, book->revision, candidate->revision, book->count,
            book->approved, book->posted, candidate->approved, candidate->posted};
        *out = review;
    } else free(review);
    free(candidate);
    return status;
}
void umi_open_tms_accounting_review_destroy(UmiOpenTmsAccountingReview *review)
{
    free(review);
}
UmiStatus umi_open_tms_accounting_review_read(const UmiOpenTmsAccountingReview *review,
    UmiOpenTmsAccountingReviewSnapshot *out)
{
    if (review == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = review->snapshot; return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_accounting_review_line(const UmiOpenTmsAccountingReview *review,
    size_t index, UmiOpenTmsAccountingLine *out)
{
    if (review == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= review->source.count) return UMI_STATUS_NOT_FOUND;
    *out = review->source.lines[index]; return UMI_STATUS_OK;
}
static int SameLine(const UmiOpenTmsAccountingLine *left, const UmiOpenTmsAccountingLine *right)
{
    return !strcmp(left->accountId, right->accountId) && !strcmp(left->currency, right->currency) &&
        !strcmp(left->referenceId, right->referenceId) && left->debit == right->debit && left->credit == right->credit;
}
UmiStatus umi_open_tms_accounting_review_apply(UmiOpenTmsAccountingBook *book,
    const UmiOpenTmsAccountingReview *review)
{
    if (review == NULL || book == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_accounting_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    const UmiOpenTmsAccountingBook *source = &review->source;
    if (book->revision != source->revision || book->count != source->count ||
        book->approved != source->approved || book->posted != source->posted)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < book->count; ++i)
        if (!SameLine(&book->lines[i], &source->lines[i])) return UMI_STATUS_INVALID_STATE;
    /* Preflight already validated the exact values and action. Publish only
     * action state, leaving all caller-owned inactive storage undisturbed. */
    book->approved = review->snapshot.approved_after;
    book->posted = review->snapshot.posted_after;
    book->revision = review->snapshot.next_revision;
    return UMI_STATUS_OK;
}
