/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/position_review.h
 * PURPOSE: Inspect owned account positions and prepare explicit quote choices from the same capture.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_POSITION_REVIEW_H
#define UMICOM_BROKER_CONNECTIVITY_POSITION_REVIEW_H
#include "umicom/broker_connectivity/quotes.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiIbkrPositionReview UmiIbkrPositionReview;
    typedef struct UmiIbkrPositionReviewSummary
    {
        char account[64];
        size_t count;
        bool complete, stale;
        UmiTradingEnvironment requestedEnvironment;
        uint64_t requestedAtMilliseconds, completedAtMilliseconds;
    } UmiIbkrPositionReviewSummary;
    /** Capture received positions by value. Partial and disconnected responses can
 * be inspected, with their state retained. Decimal provider text stays exact.
 * This performs no I/O and grants no trading authority. out_review is cleared
 * on failure. The owner serializes access and destroys the review on reconnect. */
    UmiStatus UmiIbkrPositionReviewCreate(const UmiIbkrConnectionSnapshot *snapshot,
                                          UmiIbkrPositionReview **out_review);
    void UmiIbkrPositionReviewDestroy(UmiIbkrPositionReview *review);
    UmiStatus UmiIbkrPositionReviewRead(const UmiIbkrPositionReview *review,
                                        UmiIbkrPositionReviewSummary *out_summary);
    UmiStatus UmiIbkrPositionReviewAt(const UmiIbkrPositionReview *review, size_t index,
                                      UmiIbkrPositionObservation *out_position);
    /** Prepare a contract only after rechecking a complete capture against the
 * current read-only READY connection snapshot. Account, request/completion
 * stamps and selected position must agree; timestamps are not unique session
 * tokens, so the owner must never reuse a review across connections. Missing
 * exchange, nonnumeric ID and combination contracts are refused. This only
 * copies a quote candidate; it does not validate venue permissions, resolve a
 * contract, subscribe, place an order or refresh a portfolio. Failure leaves
 * output untouched. Callers review the exchange before separately subscribing. */
    UmiStatus UmiIbkrPositionReviewQuote(const UmiIbkrPositionReview *review, size_t index,
                                         const UmiIbkrConnectionSnapshot *current,
                                         UmiIbkrQuoteContract *out_contract);
#ifdef __cplusplus
}
#endif
#endif
