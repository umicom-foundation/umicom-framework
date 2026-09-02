/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/oms.c
 *
 * PURPOSE:
 *   Coordinate order validation, risk evaluation and storage in a small reference OMS.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of oms. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/oms.h"
#include "umicom/trading/order_request.h"
#include "umicom/trading/pretrade_risk.h"

/* Initialise oms from caller-provided values so later operations receive a known state. */
void umi_oms_init(UmiOms *oms, UmiRiskLimit limit)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (oms == NULL) {
        return;
    }

    umi_order_store_init(&oms->orders);
    umi_kill_switch_reset(&oms->kill_switch);
    umi_order_throttle_init(&oms->throttle, 100U, 1000);
    oms->risk_limit = limit;
}

/* Provide the oms submit operation used by this module and its client applications. */
UmiStatus umi_oms_submit(UmiOms *oms,
                         const UmiOrderRequest *request,
                         double current_position,
                         double daily_pnl,
                         int64_t now_ms,
                         UmiRiskDecision *decision)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (oms == NULL || request == NULL || decision == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (oms->kill_switch.engaged) {
        umi_risk_decision_deny(decision, "kill switch engaged");
        return UMI_STATUS_PERMISSION_DENIED;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_order_throttle_accept(&oms->throttle, now_ms)) {
        umi_risk_decision_deny(decision, "order throttle exceeded");
        return UMI_STATUS_BUSY;
    }

    const UmiStatus validation = umi_order_request_validate(request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (validation != UMI_STATUS_OK) {
        umi_risk_decision_deny(decision, "invalid order request");
        return validation;
    }

    *decision = umi_pretrade_risk_evaluate(request,
                                           &oms->risk_limit,
                                           current_position,
                                           daily_pnl);
    /* Apply this operation only while the related capability or state is available. */
    if (!decision->allowed) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    UmiOrder order = {0};
    order.request = *request;
    order.status = UMI_ORDER_ACCEPTED;
    order.version = 1U;
    return umi_order_store_add(&oms->orders, &order);
}
