/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_pretrade_guard.c
 *
 * PURPOSE:
 *   Verify provider-neutral pre-trade risk and live-approval gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/broker_connectivity/pretrade_guard.h"

int main(void)
{
    UmiOrderRequest request = {0};
    UmiBrokerPretradeContext context = {0};
    UmiRiskDecision decision;

    request.side = UMI_SIDE_BUY;
    request.quantity = 10.0;
    request.limit_price = 100.0;
    request.environment = UMI_TRADING_PAPER;
    context.referencePrice = 100.0;
    context.buyingPower = 10000.0;
    context.trusted = 1;
    context.limits.max_order_quantity = 100.0;
    context.limits.max_order_notional = 5000.0;
    context.limits.max_position_quantity = 100.0;
    context.limits.max_daily_loss = 1000.0;

    assert(umi_broker_pretrade_guard_evaluate(
               &request, &context, &decision) == UMI_STATUS_OK);
    assert(decision.allowed);

    request.environment = UMI_TRADING_LIVE;
    assert(umi_broker_pretrade_guard_evaluate(
               &request, &context, &decision) == UMI_STATUS_OK);
    assert(!decision.allowed);
    context.liveApproved = 1;
    assert(umi_broker_pretrade_guard_evaluate(
               &request, &context, &decision) == UMI_STATUS_OK);
    assert(decision.allowed);
    return 0;
}
