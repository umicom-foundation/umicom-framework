/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/pretrade_risk.c
 *
 * PURPOSE:
 *   Evaluate quantity, notional, projected position and daily loss before an order can proceed.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of pretrade risk. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/pretrade_risk.h"
#include "umicom/trading/risk_context.h"
#include "umicom/trading/risk_decision.h"
#include "umicom/trading/risk_limit.h"

UmiRiskDecision umi_pretrade_risk_evaluate(const UmiOrderRequest *request,
                                            const UmiRiskLimit *limit,
                                            double current_position,
                                            double daily_pnl)
{
    UmiRiskDecision decision = {0};

    if (request == NULL || !umi_risk_limit_valid(limit)) {
        umi_risk_decision_deny(&decision, "invalid risk input");
        return decision;
    }

    const double reference_price =
        request->limit_price > 0.0 ? request->limit_price : 1.0;
    const double notional =
        request->quantity * reference_price * request->instrument.multiplier;
    const double projected =
        umi_risk_projected_position(current_position, request);
    const double absolute_projected =
        projected < 0.0 ? -projected : projected;

    if (request->quantity > limit->max_order_quantity) {
        umi_risk_decision_deny(&decision,
                               "maximum order quantity exceeded");
        return decision;
    }

    if (notional > limit->max_order_notional) {
        umi_risk_decision_deny(&decision,
                               "maximum order notional exceeded");
        return decision;
    }

    if (absolute_projected > limit->max_position_quantity) {
        umi_risk_decision_deny(&decision,
                               "maximum position exceeded");
        return decision;
    }

    if (daily_pnl < 0.0 && -daily_pnl > limit->max_daily_loss) {
        umi_risk_decision_deny(&decision,
                               "daily loss limit exceeded");
        return decision;
    }

    umi_risk_decision_allow(&decision);
    return decision;
}
