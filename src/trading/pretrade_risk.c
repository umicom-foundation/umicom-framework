/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/pretrade_risk.c
 *
 * PURPOSE:
 *   Evaluate quantity, notional, projected position and daily loss before an order can proceed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of pretrade risk. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/pretrade_risk.h"
#include "umicom/trading/order_request.h"
#include <math.h>
#include "umicom/trading/risk_context.h"
#include "umicom/trading/risk_decision.h"
#include "umicom/trading/risk_limit.h"

/*
 * Provide the pretrade risk evaluate operation used by this module and its client
 * applications.
 */
UmiRiskDecision umi_pretrade_risk_evaluate(const UmiOrderRequest *request,
                                            const UmiRiskLimit *limit,
                                            double current_position,
                                            double daily_pnl)
{
    UmiRiskDecision decision = {0};

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_order_request_validate(request) != UMI_STATUS_OK ||
        !umi_risk_limit_valid(limit) ||
        !isfinite(current_position) || !isfinite(daily_pnl)) {
        umi_risk_decision_deny(&decision, "invalid risk input");
        return decision;
    }

    /*
     * Preserve the existing valuation policy in this correctness update.
     * A missing positive limit price still falls back to 1.0; this is not a
     * market-data valuation and must not be treated as live market-order
     * readiness. A separate policy change needs an authorised reference price.
     */
    const double reference_price =
        request->limit_price > 0.0 ? request->limit_price : 1.0;
    const double notional =
        request->quantity * reference_price * request->instrument.multiplier;
    const double projected =
        umi_risk_projected_position(current_position, request);
    const double absolute_projected =
        projected < 0.0 ? -projected : projected;

    /*
     * Finite inputs do not guarantee finite arithmetic. Fail closed when a
     * notional multiplication or projected-position addition overflows, before
     * any limit comparison or allow decision. No caller-owned input is changed.
     */
    if (!isfinite(notional) || !isfinite(projected)) {
        umi_risk_decision_deny(&decision, "non-finite risk calculation");
        return decision;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (request->quantity > limit->max_order_quantity) {
        umi_risk_decision_deny(&decision,
                               "maximum order quantity exceeded");
        return decision;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (notional > limit->max_order_notional) {
        umi_risk_decision_deny(&decision,
                               "maximum order notional exceeded");
        return decision;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (absolute_projected > limit->max_position_quantity) {
        umi_risk_decision_deny(&decision,
                               "maximum position exceeded");
        return decision;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (daily_pnl < 0.0 && -daily_pnl > limit->max_daily_loss) {
        umi_risk_decision_deny(&decision,
                               "daily loss limit exceeded");
        return decision;
    }

    umi_risk_decision_allow(&decision);
    return decision;
}
