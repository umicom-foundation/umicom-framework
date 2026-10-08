/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/full_quantity/fixture.h
 * PURPOSE: Supply invented, transport-free liquidity observations for regression coverage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FULL_QUANTITY_TEST_FIXTURE_H
#define UMICOM_FULL_QUANTITY_TEST_FIXTURE_H
#include "umicom/broker_connectivity/fill_policy.h"
#include "umicom/trading/fill_watch.h"
#include <stdio.h>
#include <string.h>
#define CHECK(expression)                                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        if (!(expression))                                                                                   \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #expression);                                              \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static inline UmiDecimal Amount(int64_t coefficient, uint8_t scale)
{
    UmiDecimal value = {0};
    value.coefficient = coefficient;
    value.scale = scale;
    return value;
}
static inline UmiFullQuantityPolicy Policy(void)
{
    UmiFullQuantityPolicy policy = {0};
    policy.mode = UMI_FULL_QUANTITY_WATCH_THEN_IMMEDIATE;
    policy.side = UMI_SIDE_BUY;
    policy.waitingTimeInForce = UMI_TIF_DAY;
    policy.quantity = Amount(7000, 0);
    policy.limitPrice = Amount(425, 2);
    policy.extraVisibleQuantity = Amount(0, 0);
    policy.maximumAgeMilliseconds = 1500U;
    policy.maximumSkewMilliseconds = 500U;
    return policy;
}
static inline UmiLiquidityObservation Observation(void)
{
    UmiLiquidityObservation value = {0};
    value.available = true;
    value.realtime = true;
    value.price = Amount(425, 2);
    value.visibleQuantity = Amount(7000, 0);
    value.priceReceivedAtMilliseconds = 9000U;
    value.sizeReceivedAtMilliseconds = 9000U;
    return value;
}
static inline UmiIbkrQuoteSnapshot Quote(void)
{
    UmiIbkrQuoteSnapshot quote = {0};
    quote.requestId = 101U;
    quote.contract.contractId = 12345U;
    strcpy(quote.contract.exchange, "SMART");
    quote.subscribed = true;
    quote.dataType = UMI_IBKR_DATA_REALTIME;
    UmiIbkrQuoteValue value = {0};
    value.received = true;
    value.dataType = UMI_IBKR_DATA_REALTIME;
    value.receivedAtMilliseconds = 9000U;
    strcpy(value.text, "4.25");
    quote.ask = value;
    quote.bid = value;
    strcpy(value.text, "7000");
    quote.askSize = value;
    quote.bidSize = value;
    return quote;
}
#endif
