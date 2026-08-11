/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/oms.c
 *
 * PURPOSE:
 *   Coordinate order validation, risk evaluation and storage in a small reference OMS.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of oms. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/oms.h"
#include "umicom/trading/order_request.h"
#include "umicom/trading/pretrade_risk.h"

void umi_oms_init(UmiOms *oms, UmiRiskLimit limit)
{
    if (oms == NULL) {
        return;
    }

    umi_order_store_init(&oms->orders);
    umi_kill_switch_reset(&oms->kill_switch);
    umi_order_throttle_init(&oms->throttle, 100U, 1000);
    oms->risk_limit = limit;
}

UmiStatus umi_oms_submit(UmiOms *oms,
                         const UmiOrderRequest *request,
                         double current_position,
                         double daily_pnl,
                         int64_t now_ms,
                         UmiRiskDecision *decision)
{
    if (oms == NULL || request == NULL || decision == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (oms->kill_switch.engaged) {
        umi_risk_decision_deny(decision, "kill switch engaged");
        return UMI_STATUS_PERMISSION_DENIED;
    }

    if (!umi_order_throttle_accept(&oms->throttle, now_ms)) {
        umi_risk_decision_deny(decision, "order throttle exceeded");
        return UMI_STATUS_BUSY;
    }

    const UmiStatus validation = umi_order_request_validate(request);
    if (validation != UMI_STATUS_OK) {
        umi_risk_decision_deny(decision, "invalid order request");
        return validation;
    }

    *decision = umi_pretrade_risk_evaluate(request,
                                           &oms->risk_limit,
                                           current_position,
                                           daily_pnl);
    if (!decision->allowed) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    UmiOrder order = {0};
    order.request = *request;
    order.status = UMI_ORDER_ACCEPTED;
    order.version = 1U;
    return umi_order_store_add(&oms->orders, &order);
}
