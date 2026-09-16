/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/pretrade_risk.h
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
 * This public contract isolates pretrade risk behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_PRETRADE_RISK_H
#define INCLUDE_UMICOM_TRADING_PRETRADE_RISK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the pretrade risk evaluate operation used by this module and its client
 * applications.
 */
UmiRiskDecision umi_pretrade_risk_evaluate(const UmiOrderRequest *request, const UmiRiskLimit *limit, double current_position, double daily_pnl);

/** Price policy for this positive-price, single-instrument reference risk model.
 * The caller supplies one clock domain for quote and evaluation timestamps.
 * These development defaults are not a venue or regulatory risk policy. */
typedef struct UmiRiskPricePolicy {
    int64_t maxQuoteAgeMs;
    double priceBufferBps;
} UmiRiskPricePolicy;

typedef enum UmiRiskPriceSource {
    UMI_RISK_PRICE_NONE = 0,
    UMI_RISK_PRICE_LIMIT = 1,
    UMI_RISK_PRICE_ASK = 2,
    UMI_RISK_PRICE_BID = 3,
    UMI_RISK_PRICE_STOP = 4
} UmiRiskPriceSource;

/** A copied calculation, not an approval token. Submission must re-evaluate. */
typedef struct UmiPretradeRiskEvidence {
    UmiRiskDecision decision;
    UmiRiskPriceSource priceSource;
    double referencePrice;
    double notional;
    double projectedPosition;
    int64_t quoteTimeMs;
    int64_t evaluatedAtMs;
    int64_t quoteAgeMs;
    int hasValuation;
} UmiPretradeRiskEvidence;

/** Five-second quote age and no extra price buffer; replace with reviewed policy. */
UmiRiskPricePolicy UmiRiskPricePolicyDefault(void);
int UmiRiskPricePolicyValid(const UmiRiskPricePolicy *policy);
const char *UmiRiskPriceSourceText(UmiRiskPriceSource source);
/** Compare validated identity AND contract metadata, not identity alone. */
int UmiRiskInstrumentMatches(const UmiInstrument *left, const UmiInstrument *right);
/** Limit orders use the limit. Market and stop orders require a matching,
 * fresh, two-sided quote with positive size on the execution side. Stops use
 * the greater of the quote-side price and trigger before the policy buffer.
 * The estimate is not a guaranteed fill price, margin model or execution gate.
 * All input pointers are borrowed for the call; output evidence is copied. */
UmiRiskDecision UmiPretradeRiskEvaluateQuoted(
    const UmiOrderRequest *request, const UmiRiskLimit *limit,
    double currentPosition, double dailyPnl, const UmiQuote *quote,
    int64_t nowMs, const UmiRiskPricePolicy *policy,
    UmiPretradeRiskEvidence *outEvidence);

#ifdef __cplusplus
}
#endif
#endif
