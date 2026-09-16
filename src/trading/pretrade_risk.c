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
#include <string.h>
#include "umicom/trading/instrument.h"
#include "umicom/trading/quote.h"
#include "umicom/trading/risk_context.h"
#include "umicom/trading/risk_decision.h"
#include "umicom/trading/risk_limit.h"

/*
 * Provide the pretrade risk evaluate operation used by this module and its client
 * applications.
 */
static UmiRiskDecision EvaluateAtPrice(const UmiOrderRequest *request,
                                            const UmiRiskLimit *limit,
                                            double current_position,
                                            double daily_pnl,
                                            double reference_price,
                                            UmiPretradeRiskEvidence *evidence)
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

    /* The caller has resolved a real limit or an explicitly checked quote.
     * There is no synthetic price of 1.0 for an unpriced market order. */
    if (!isfinite(reference_price) || reference_price <= 0.0) {
        umi_risk_decision_deny(&decision, "a valid reference price is required");
        return decision;
    }
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

    if (evidence != NULL) {
        evidence->referencePrice = reference_price;
        evidence->notional = notional;
        evidence->projectedPosition = projected;
        evidence->hasValuation = 1;
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

/* The original API remains available for priced orders. Callers of market or
 * stop orders now receive a denial rather than an invented valuation. */
UmiRiskDecision umi_pretrade_risk_evaluate(const UmiOrderRequest *request,
                                            const UmiRiskLimit *limit,
                                            double current_position,
                                            double daily_pnl)
{
    return UmiPretradeRiskEvaluateQuoted(request, limit, current_position,
        daily_pnl, NULL, 0, NULL, NULL);
}

UmiRiskPricePolicy UmiRiskPricePolicyDefault(void)
{
    const UmiRiskPricePolicy policy = {5000, 0.0};
    return policy;
}

int UmiRiskPricePolicyValid(const UmiRiskPricePolicy *policy)
{
    return policy != NULL && policy->maxQuoteAgeMs > 0 &&
        isfinite(policy->priceBufferBps) && policy->priceBufferBps >= 0.0 &&
        policy->priceBufferBps <= 10000.0;
}

const char *UmiRiskPriceSourceText(UmiRiskPriceSource source)
{
    switch (source) {
    case UMI_RISK_PRICE_LIMIT: return "limit price";
    case UMI_RISK_PRICE_ASK: return "quote ask";
    case UMI_RISK_PRICE_BID: return "quote bid";
    case UMI_RISK_PRICE_STOP: return "stop trigger";
    default: return "no valuation";
    }
}

int UmiRiskInstrumentMatches(const UmiInstrument *left, const UmiInstrument *right)
{
    return umi_instrument_valid(left) && umi_instrument_valid(right) &&
        umi_instrument_same(left, right) &&
        strcmp(left->symbol, right->symbol) == 0 &&
        strcmp(left->venue, right->venue) == 0 &&
        strcmp(left->currency.code, right->currency.code) == 0 &&
        left->multiplier == right->multiplier &&
        left->expiry_yyyymmdd == right->expiry_yyyymmdd;
}

/* Each return publishes the same complete evidence, including denials. */
UmiRiskDecision UmiPretradeRiskEvaluateQuoted(
    const UmiOrderRequest *request, const UmiRiskLimit *limit,
    double currentPosition, double dailyPnl, const UmiQuote *quote,
    int64_t nowMs, const UmiRiskPricePolicy *policy,
    UmiPretradeRiskEvidence *outEvidence)
{
    UmiPretradeRiskEvidence evidence = {0};
    UmiRiskPricePolicy defaults = UmiRiskPricePolicyDefault();
    const UmiRiskPricePolicy *effective = policy != NULL ? policy : &defaults;
    const char *failure = NULL;
    double price = 0.0;
    evidence.evaluatedAtMs = nowMs;
    if (umi_order_request_validate(request) != UMI_STATUS_OK ||
        !umi_risk_limit_valid(limit) || !isfinite(currentPosition) ||
        !isfinite(dailyPnl) || nowMs < 0 || !UmiRiskPricePolicyValid(effective)) {
        failure = "invalid risk input";
    } else if (request->type == UMI_ORDER_LIMIT ||
               request->type == UMI_ORDER_STOP_LIMIT) {
        price = request->limit_price;
        evidence.priceSource = UMI_RISK_PRICE_LIMIT;
    } else if (quote == NULL) {
        failure = "market and stop orders require a current quote";
    } else if (!umi_quote_valid(quote) ||
               !UmiRiskInstrumentMatches(&request->instrument, &quote->instrument)) {
        failure = "quote does not match the order instrument";
    } else if (quote->event_time_ms < 0 || quote->event_time_ms > nowMs) {
        failure = "quote timestamp is invalid or in the future";
    } else if (nowMs - quote->event_time_ms > effective->maxQuoteAgeMs) {
        failure = "quote is too old for risk valuation";
    } else if ((request->side == UMI_SIDE_BUY && quote->ask_size <= 0.0) ||
               (request->side == UMI_SIDE_SELL && quote->bid_size <= 0.0)) {
        failure = "quote has no size on the execution side";
    } else {
        evidence.quoteTimeMs = quote->event_time_ms;
        evidence.quoteAgeMs = nowMs - quote->event_time_ms;
        price = request->side == UMI_SIDE_BUY ? quote->ask : quote->bid;
        evidence.priceSource = request->side == UMI_SIDE_BUY
            ? UMI_RISK_PRICE_ASK : UMI_RISK_PRICE_BID;
        if (request->type == UMI_ORDER_STOP && request->stop_price > price) {
            price = request->stop_price;
            evidence.priceSource = UMI_RISK_PRICE_STOP;
        }
        price *= 1.0 + effective->priceBufferBps / 10000.0;
    }
    if (failure != NULL) {
        umi_risk_decision_deny(&evidence.decision, failure);
    } else {
        evidence.decision = EvaluateAtPrice(request, limit, currentPosition,
            dailyPnl, price, &evidence);
    }
    if (outEvidence != NULL) *outEvidence = evidence;
    return evidence.decision;
}
