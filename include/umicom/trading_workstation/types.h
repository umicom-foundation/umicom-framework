/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_workstation/types.h
 *
 * PURPOSE:
 *   Define bounded professional trading-workstation evidence used by watchlist,
 *   depth, charts, Time and Sales, execution, portfolio, research and linked
 *   context projections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_WORKSTATION_TYPES_H
#define UMICOM_TRADING_WORKSTATION_TYPES_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TRADING_WORKSTATION_ID_CAPACITY 96U
#define UMI_TRADING_WORKSTATION_LABEL_CAPACITY 128U

typedef struct UmiTradingProfessionalInput {
    const char *label;

    double bid;
    double ask;
    double last;
    double previousClose;
    double open;
    double high;
    double low;
    double tickSize;

    double bidSize;
    double askSize;
    double bidDepth;
    double askDepth;
    double depthImbalance;
    double topLiquidity;

    double volume;
    double averageVolume;
    double tradeRate;
    double averageTradeRate;
    double latestTradeSize;
    double averageTradeSize;

    double positionQuantity;
    double averageCost;
    double marketPrice;
    double realisedPnl;
    double grossExposure;
    double netExposure;
    double largestPositionExposure;

    double marginUsed;
    double marginLimit;
    double buyingPowerUsed;
    double buyingPowerLimit;
    double riskUsed;
    double riskLimit;

    double orderQuantity;
    double orderedQuantity;
    double filledQuantity;
    double limitPrice;
    double stopPrice;
    double referencePrice;

    double alertValue;
    double alertThreshold;
    double momentumScore;
    double volatilityScore;
    double liquidityScore;
    double spreadQualityScore;
    double eventImpact;
    double minutesToEvent;
    double researchConfidence;

    uint64_t count;
    uint64_t visibleCount;
    uint64_t capacity;
    uint64_t alerts;
    uint64_t activeAlerts;
    uint64_t unacknowledgedAlerts;
    uint64_t ageMilliseconds;

    int marketDataReady;
    int brokerReady;
    int riskReady;
    int healthReady;
    int hasInstrument;
    int hasQuote;
    int hasDepth;
    int hasBar;
    int tradeTapeReady;
    int linkedContextReady;
    int trusted;
    int active;
    int killSwitchEngaged;
    int canPreviewOrder;
    int canSubmitOrder;
    int canCancelOrder;

    uint64_t revision;
} UmiTradingProfessionalInput;

typedef struct UmiTradingProfessionalSnapshot {
    char id[UMI_TRADING_WORKSTATION_ID_CAPACITY];
    char label[UMI_TRADING_WORKSTATION_LABEL_CAPACITY];
    double value;
    double secondaryValue;
    double score;
    double ratio;
    double notional;
    double pnl;
    int ready;
    int attention;
    int blocked;
    uint64_t revision;
} UmiTradingProfessionalSnapshot;

typedef UmiStatus (*UmiTradingProfessionalEvaluator)(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot);

void umi_trading_professional_input_init(
    UmiTradingProfessionalInput *input);
void umi_trading_professional_snapshot_init(
    UmiTradingProfessionalSnapshot *snapshot);
UmiStatus umi_trading_professional_snapshot_validate(
    const UmiTradingProfessionalSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
