/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_workstation/test_chart_tape_trade_size_signal.c
 *
 * PURPOSE:
 *   Verify the Trade Size Signal professional workstation calculation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <math.h>
#include <string.h>

#include "umicom/trading_workstation/chart_tape/trade_size_signal.h"

int main(void)
{
    UmiTradingProfessionalInput input;
    UmiTradingProfessionalSnapshot snapshot;

    umi_trading_professional_input_init(&input);
    input.bid = 99.90;
    input.ask = 100.10;
    input.last = 100.00;
    input.previousClose = 98.00;
    input.open = 98.50;
    input.high = 102.00;
    input.low = 97.00;
    input.tickSize = 0.01;
    input.bidSize = 120.0;
    input.askSize = 80.0;
    input.bidDepth = 5200.0;
    input.askDepth = 4800.0;
    input.depthImbalance = 0.04;
    input.topLiquidity = 10000.0;
    input.volume = 1500000.0;
    input.averageVolume = 1000000.0;
    input.tradeRate = 25.0;
    input.averageTradeRate = 20.0;
    input.latestTradeSize = 250.0;
    input.averageTradeSize = 200.0;
    input.positionQuantity = 100.0;
    input.averageCost = 97.50;
    input.marketPrice = 100.00;
    input.realisedPnl = 125.0;
    input.grossExposure = 25000.0;
    input.netExposure = 10000.0;
    input.largestPositionExposure = 10000.0;
    input.marginUsed = 20000.0;
    input.marginLimit = 100000.0;
    input.buyingPowerUsed = 20000.0;
    input.buyingPowerLimit = 100000.0;
    input.riskUsed = 25000.0;
    input.riskLimit = 100000.0;
    input.orderQuantity = 10.0;
    input.orderedQuantity = 10.0;
    input.filledQuantity = 5.0;
    input.limitPrice = 99.50;
    input.stopPrice = 98.00;
    input.referencePrice = 100.00;
    input.alertValue = 100.00;
    input.alertThreshold = 101.00;
    input.momentumScore = 35.0;
    input.volatilityScore = 42.0;
    input.liquidityScore = 85.0;
    input.spreadQualityScore = 80.0;
    input.eventImpact = 40.0;
    input.minutesToEvent = 120.0;
    input.researchConfidence = 80.0;
    input.count = 10U;
    input.visibleCount = 8U;
    input.capacity = 100U;
    input.alerts = 4U;
    input.activeAlerts = 2U;
    input.unacknowledgedAlerts = 0U;
    input.ageMilliseconds = 100U;
    input.marketDataReady = 1;
    input.brokerReady = 1;
    input.riskReady = 1;
    input.healthReady = 1;
    input.hasInstrument = 1;
    input.hasQuote = 1;
    input.hasDepth = 1;
    input.hasBar = 1;
    input.tradeTapeReady = 1;
    input.linkedContextReady = 1;
    input.trusted = 1;
    input.active = 1;
    input.killSwitchEngaged = 0;
    input.canPreviewOrder = 1;
    input.canSubmitOrder = 1;
    input.canCancelOrder = 1;
    input.revision = 55U;

    assert(umi_trading_professional_evaluate_chart_tape_trade_size_signal(&input, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.id, "chart_tape.trade-size-signal") == 0);
    assert(snapshot.revision == 55U);
    assert(snapshot.score >= 0.0 && snapshot.score <= 100.0);
    assert(snapshot.blocked == 0);
    assert(snapshot.ready == 1 || snapshot.attention == 0 || snapshot.value >= 0.0);
    return 0;
}
