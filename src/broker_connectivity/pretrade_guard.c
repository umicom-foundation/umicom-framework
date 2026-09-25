/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/pretrade_guard.c
 *
 * PURPOSE:
 *   Implement provider-neutral pre-trade safety gates before broker submit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/pretrade_guard.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static UmiStatus deny(UmiRiskDecision *decision, const char *reason)
{
    decision->allowed = 0;
    (void)snprintf(decision->reason, sizeof(decision->reason), "%s", reason);
    return UMI_STATUS_OK;
}

UmiStatus umi_broker_pretrade_guard_evaluate(
    const UmiOrderRequest *request,
    const UmiBrokerPretradeContext *context,
    UmiRiskDecision *outDecision)
{
    double price;
    double notional;
    double projectedPosition;

    if (request == NULL || context == NULL || outDecision == NULL ||
        request->quantity <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(outDecision, 0, sizeof(*outDecision));
    if (!context->trusted) return deny(outDecision, "Trading context is not trusted.");
    if (context->killSwitchEngaged) return deny(outDecision, "Kill switch is engaged.");
    if (context->restrictedInstrument) return deny(outDecision, "Instrument is restricted.");
    if (request->environment == UMI_TRADING_LIVE && !context->liveApproved) {
        return deny(outDecision, "Live trading is not approved.");
    }

    price = request->limit_price > 0.0
        ? request->limit_price : context->referencePrice;
    if (price <= 0.0) return deny(outDecision, "No valid reference price.");

    notional = request->quantity * price;
    projectedPosition = context->currentPositionQuantity +
        (request->side == UMI_SIDE_BUY ? request->quantity : -request->quantity);

    if (context->limits.max_order_quantity > 0.0 &&
        request->quantity > context->limits.max_order_quantity) {
        return deny(outDecision, "Order quantity exceeds risk limit.");
    }
    if (context->limits.max_order_notional > 0.0 &&
        notional > context->limits.max_order_notional) {
        return deny(outDecision, "Order notional exceeds risk limit.");
    }
    if (context->limits.max_position_quantity > 0.0 &&
        fabs(projectedPosition) > context->limits.max_position_quantity) {
        return deny(outDecision, "Projected position exceeds risk limit.");
    }
    if (context->limits.max_daily_loss > 0.0 &&
        context->dailyPnl <= -context->limits.max_daily_loss) {
        return deny(outDecision, "Daily loss boundary has been reached.");
    }
    if (context->buyingPower > 0.0 && notional > context->buyingPower) {
        return deny(outDecision, "Insufficient buying power.");
    }

    outDecision->allowed = 1;
    (void)snprintf(outDecision->reason, sizeof(outDecision->reason),
                   "%s", "Pre-trade checks passed.");
    return UMI_STATUS_OK;
}
