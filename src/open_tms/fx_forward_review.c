/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/fx_forward_review.c
 *
 * PURPOSE:
 *   Prepare and apply immutable local FX-forward transition reviews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/open_tms/fx_forward_review.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Reuse the canonical lifecycle implementation for both preparation and apply.
 * The opaque owner keeps caller-mutated display values out of the authority path.
 * Exact active-state comparison also catches direct edits to the public book
 * that accidentally omit its revision increment. It is not a locking primitive. */
struct UmiOpenTmsFxForwardReview {
    UmiOpenTmsFxForwardBook before;
    UmiOpenTmsFxForwardReviewSnapshot snapshot;
};
static bool ReviewTradeEqual(const UmiOpenTmsFxForward *a, const UmiOpenTmsFxForward *b) {
    return strcmp(a->tradeId, b->tradeId) == 0 &&
        strcmp(a->accountId, b->accountId) == 0 &&
        strcmp(a->counterpartyId, b->counterpartyId) == 0 &&
        strcmp(a->baseCurrency, b->baseCurrency) == 0 &&
        strcmp(a->quoteCurrency, b->quoteCurrency) == 0 &&
        a->baseNotional == b->baseNotional && a->forwardRate == b->forwardRate &&
        a->marketForwardRate == b->marketForwardRate &&
        a->maturityMilliseconds == b->maturityMilliseconds &&
        a->state == b->state && a->version == b->version;
}
UmiStatus umi_open_tms_fx_forward_review_create(
    const UmiOpenTmsFxForwardBook *book, const char *tradeId,
    UmiOpenTmsFxForwardState nextState, uint64_t nextVersion,
    UmiOpenTmsFxForwardReview **outReview) {
    if (outReview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReview = NULL;
    if (book == NULL || tradeId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiOpenTmsFxForwardBook *candidate = malloc(sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(candidate, book, sizeof *candidate);
    UmiStatus status = umi_open_tms_fx_forward_book_transition(candidate, tradeId, nextState, nextVersion);
    if (status != UMI_STATUS_OK) { free(candidate); return status; }
    UmiOpenTmsFxForwardReview *review = calloc(1U, sizeof *review);
    if (review == NULL) { free(candidate); return UMI_STATUS_OUT_OF_MEMORY; }
    memcpy(&review->before, book, sizeof review->before);
    review->snapshot.beforeRevision = book->revision;
    review->snapshot.afterRevision = candidate->revision;
    review->snapshot.recordCount = book->count;
    /* Transition validation above establishes terminated, unique identities. */
    const UmiOpenTmsFxForward *before = umi_open_tms_fx_forward_book_find(book, tradeId);
    const UmiOpenTmsFxForward *after = umi_open_tms_fx_forward_book_find(candidate, tradeId);
    if (before == NULL || after == NULL) {
        free(review); free(candidate); return UMI_STATUS_INVALID_STATE;
    }
    review->snapshot.before = *before;
    review->snapshot.after = *after;
    free(candidate);
    *outReview = review;
    return UMI_STATUS_OK;
}
void umi_open_tms_fx_forward_review_destroy(UmiOpenTmsFxForwardReview *review) {
    free(review);
}
UmiStatus umi_open_tms_fx_forward_review_read(
    const UmiOpenTmsFxForwardReview *review, UmiOpenTmsFxForwardReviewSnapshot *outSnapshot) {
    if (review == NULL || outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSnapshot = review->snapshot;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_fx_forward_review_apply(
    UmiOpenTmsFxForwardBook *book, const UmiOpenTmsFxForwardReview *review) {
    if (book == NULL || review == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (book->revision != review->before.revision || book->count != review->before.count)
        return UMI_STATUS_BUSY;
    /* The public find validates every active row before bounded string
     * comparisons below. A malformed current book is a stale review, not a
     * reason to inspect beyond arrays or silently repair caller-owned values. */
    if (umi_open_tms_fx_forward_book_find(book, review->snapshot.before.tradeId) == NULL)
        return UMI_STATUS_BUSY;
    for (size_t i = 0U; i < book->count; ++i)
        if (!ReviewTradeEqual(&book->trades[i], &review->before.trades[i])) return UMI_STATUS_BUSY;
    return umi_open_tms_fx_forward_book_transition(book, review->snapshot.before.tradeId,
        review->snapshot.after.state, review->snapshot.after.version);
}
