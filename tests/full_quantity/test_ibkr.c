/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/full_quantity/test_ibkr.c
 * PURPOSE: Ensure IBKR field previews retain full-quantity intent and quote bindings reject substitutions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiFullQuantityPolicy policy = Policy();
    UmiIbkrQuoteSnapshot quote = Quote();
    UmiIbkrQuoteContract contract = quote.contract;
    UmiFullQuantityReview review;
    memset(&review, 0x5a, sizeof review);
    unsigned char before[sizeof review];
    memcpy(before, &review, sizeof review);
    if (!strcmp(argv[1], "fields"))
    {
        UmiIbkrFillInstruction fields;
        CHECK(UmiIbkrDescribeFillInstruction(&policy, &fields) == UMI_STATUS_OK);
        CHECK(!strcmp(fields.orderType, "LMT") && !strcmp(fields.timeInForce, "FOK"));
        CHECK(!fields.allOrNone && !fields.minimumQuantitySet && fields.waitForDisplayedLiquidity);
        policy.mode = UMI_FULL_QUANTITY_IMMEDIATE;
        CHECK(UmiIbkrDescribeFillInstruction(&policy, &fields) == UMI_STATUS_OK);
        CHECK(!fields.waitForDisplayedLiquidity && !strcmp(fields.timeInForce, "FOK"));
        policy.mode = UMI_FULL_QUANTITY_WAIT;
        CHECK(UmiIbkrDescribeFillInstruction(&policy, &fields) == UMI_STATUS_OK);
        CHECK(fields.allOrNone && !fields.minimumQuantitySet && !strcmp(fields.timeInForce, "DAY"));
        policy.waitingTimeInForce = UMI_TIF_GTC;
        CHECK(UmiIbkrDescribeFillInstruction(&policy, &fields) == UMI_STATUS_OK);
        CHECK(fields.allOrNone && !strcmp(fields.timeInForce, "GTC"));
        return 0;
    }
    UmiStatus expectedStatus = UMI_STATUS_OK;
    UmiLiquidityAssessment expected = UMI_LIQUIDITY_MEETS_DISPLAYED_RULE;
    if (!strcmp(argv[1], "contract"))
    {
        ++contract.contractId;
        expectedStatus = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(argv[1], "before-subscription"))
    {
        quote.requestedAtMilliseconds = 9001U;
        expectedStatus = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(argv[1], "zero-request"))
    {
        quote.requestId = 0U;
        expectedStatus = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(argv[1], "exchange"))
    {
        strcpy(contract.exchange, "LSE");
        expectedStatus = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(argv[1], "request"))
    {
        ++quote.requestId;
        expectedStatus = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(argv[1], "unterminated"))
    {
        memset(quote.contract.exchange, 'x', sizeof quote.contract.exchange);
        expectedStatus = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(argv[1], "unterminated-value"))
    {
        memset(quote.ask.text, '1', sizeof quote.ask.text);
        expectedStatus = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(argv[1], "exponent"))
    {
        strcpy(quote.ask.text, "4.25e0");
        expectedStatus = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(argv[1], "excess-scale"))
    {
        strcpy(quote.ask.text, "4.2500000000");
        expectedStatus = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (!strcmp(argv[1], "cancelled"))
    {
        quote.subscribed = false;
        expected = UMI_LIQUIDITY_UNAVAILABLE;
    }
    else if (!strcmp(argv[1], "failed"))
    {
        quote.failed = true;
        expected = UMI_LIQUIDITY_UNAVAILABLE;
    }
    else if (!strcmp(argv[1], "missing-size"))
    {
        quote.askSize.received = false;
        expected = UMI_LIQUIDITY_UNAVAILABLE;
    }
    else if (!strcmp(argv[1], "unavailable"))
    {
        quote.ask.unavailable = true;
        expected = UMI_LIQUIDITY_UNAVAILABLE;
    }
    else if (!strcmp(argv[1], "frozen"))
    {
        quote.dataType = UMI_IBKR_DATA_FROZEN;
        expected = UMI_LIQUIDITY_NOT_REALTIME;
    }
    else if (!strcmp(argv[1], "mixed-type"))
    {
        quote.askSize.dataType = UMI_IBKR_DATA_DELAYED;
        expected = UMI_LIQUIDITY_NOT_REALTIME;
    }
    else if (!strcmp(argv[1], "stale"))
    {
        quote.askSize.stale = true;
        expected = UMI_LIQUIDITY_STALE;
    }
    else if (!strcmp(argv[1], "sell-side"))
    {
        policy.side = UMI_SIDE_SELL;
        strcpy(quote.bidSize.text, "10");
        expected = UMI_LIQUIDITY_INSUFFICIENT;
    }
    else if (!strcmp(argv[1], "other-side"))
    {
        strcpy(quote.bidSize.text, "10");
        quote.bid.stale = true;
    }
    else if (!strcmp(argv[1], "large-whole"))
    {
        policy.quantity = Amount(INT64_MAX, 0);
        strcpy(quote.askSize.text, "9223372036854775807");
    }
    else if (strcmp(argv[1], "match"))
        return 2;
    CHECK(UmiIbkrReviewFullQuantity(&policy, &contract, 101U, &quote, 10000U, &review) == expectedStatus);
    if (expectedStatus == UMI_STATUS_OK)
        CHECK(review.assessment == expected);
    else
        CHECK(!memcmp(&before, &review, sizeof review));
    return 0;
}
