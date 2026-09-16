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
static UmiStatus SubmitOrder(UmiOms *oms,
                         const UmiOrderRequest *request,
                         double current_position,
                         double daily_pnl,
                         int64_t now_ms,
                         UmiRiskDecision *decision,
                         const UmiQuote *quote,
                         const UmiRiskPricePolicy *pricePolicy,
                         UmiPretradeRiskEvidence *outEvidence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (oms == NULL || request == NULL || decision == NULL || now_ms < 0) {
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

    *decision = UmiPretradeRiskEvaluateQuoted(request, &oms->risk_limit,
        current_position, daily_pnl, quote, now_ms, pricePolicy, outEvidence);
    /* Apply this operation only while the related capability or state is available. */
    if (!decision->allowed) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    UmiOrder order = {0};
    order.request = *request;
    order.status = UMI_ORDER_ACCEPTED;
    order.version = 1U;
    const UmiStatus stored = umi_order_store_add(&oms->orders, &order);
    if (stored != UMI_STATUS_OK) {
        umi_risk_decision_deny(decision,
            stored == UMI_STATUS_ALREADY_EXISTS
                ? "client order identifier already exists" : "order could not be stored");
        if (outEvidence != NULL) outEvidence->decision = *decision;
    }
    return stored;
}

/* Existing priced-order callers retain their entry point and safeguards. */
UmiStatus umi_oms_submit(UmiOms *oms, const UmiOrderRequest *request,
                         double current_position, double daily_pnl,
                         int64_t now_ms, UmiRiskDecision *decision)
{
    return SubmitOrder(oms, request, current_position, daily_pnl, now_ms,
        decision, NULL, NULL, NULL);
}

/* Quote and policy are borrowed only for this submission. They are never
 * substituted into the order's economic limit_price or stop_price fields. */
UmiStatus UmiOmsSubmitQuoted(UmiOms *oms, const UmiOrderRequest *request,
    double currentPosition, double dailyPnl, int64_t nowMs,
    const UmiQuote *quote, const UmiRiskPricePolicy *policy,
    UmiRiskDecision *decision, UmiPretradeRiskEvidence *outEvidence)
{
    UmiPretradeRiskEvidence evidence = {0};
    UmiRiskDecision local = {0};
    UmiRiskDecision *result = decision != NULL ? decision : &local;
    umi_risk_decision_deny(result, "invalid order submission");
    const UmiStatus status = SubmitOrder(oms, request, currentPosition,
        dailyPnl, nowMs, result, quote, policy, &evidence);
    evidence.decision = *result;
    if (outEvidence != NULL) *outEvidence = evidence;
    return status;
}
