/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/fill_policy.h
 * PURPOSE: Bind full-quantity reviews to an exact IBKR quote subscription and describe requested API fields.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_FILL_POLICY_H
#define UMICOM_BROKER_CONNECTIVITY_FILL_POLICY_H
#include "umicom/broker_connectivity/quotes.h"
#include "umicom/trading/fill_policy.h"
#ifdef __cplusplus
extern "C"
{
#endif

    /* A field preview is descriptive, not a wire message or a supported-route
 * claim. There is deliberately no transmit flag, broker callback or account
 * authority in this value. minimumQuantitySet stays false: minQty is not a
 * substitute for an enforceable full-quantity restriction. */
    typedef struct UmiIbkrFillInstruction
    {
        char orderType[8], timeInForce[8];
        bool allOrNone, minimumQuantitySet, waitForDisplayedLiquidity;
    } UmiIbkrFillInstruction;
    UmiStatus UmiIbkrDescribeFillInstruction(const UmiFullQuantityPolicy *policy,
                                             UmiIbkrFillInstruction *out);

    /* Convert one bound quote into toolkit-neutral evidence for a local watch.
 * Missing/failed/cancelled fields produce available=false. No live session is
 * borrowed or retained, and no provider operation is issued. */
    UmiStatus UmiIbkrFillObservation(UmiSide side, const UmiIbkrQuoteContract *expectedContract,
                                     uint32_t expectedRequestId, const UmiIbkrQuoteSnapshot *quote,
                                     UmiLiquidityObservation *out);

    /* The caller must retire the expectation on reconnect: request IDs are unique
 * only within one connection. An exact contract/exchange and request ID match
 * is required; another subscription's evidence is never substituted. Missing
 * data produces an unavailable review. Decimal text with more than nine
 * fractional digits or outside the supported coefficient range is refused.
 * This review does not infer size units, tick rules, account permissions or
 * route support from market-data subscriptions. Output changes only on OK. */
    UmiStatus UmiIbkrReviewFullQuantity(const UmiFullQuantityPolicy *policy,
                                        const UmiIbkrQuoteContract *expectedContract,
                                        uint32_t expectedRequestId, const UmiIbkrQuoteSnapshot *quote,
                                        uint64_t nowMilliseconds, UmiFullQuantityReview *out);

#ifdef __cplusplus
}
#endif
#endif
