/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_workstation/test_service_catalogue.c
 *
 * PURPOSE:
 *   Verify all professional trading-workstation calculations are discoverable
 *   and evaluate through one Framework service catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/trading_workstation/service.h"

int main(void)
{
    UmiTradingProfessionalInput input;
    UmiTradingProfessionalSnapshot snapshot;
    size_t index;

    umi_trading_professional_input_init(&input);
    input.bid = 99.9;
    input.ask = 100.1;
    input.last = 100.0;
    input.previousClose = 99.0;
    input.high = 102.0;
    input.low = 97.0;
    input.tickSize = 0.01;
    input.bidSize = 120.0;
    input.askSize = 80.0;
    input.bidDepth = 5000.0;
    input.askDepth = 4500.0;
    input.depthImbalance = 0.05;
    input.topLiquidity = 9500.0;
    input.volume = 1500000.0;
    input.averageVolume = 1000000.0;
    input.tradeRate = 25.0;
    input.averageTradeRate = 20.0;
    input.positionQuantity = 50.0;
    input.averageCost = 98.0;
    input.marketPrice = 100.0;
    input.realisedPnl = 100.0;
    input.grossExposure = 25000.0;
    input.netExposure = 10000.0;
    input.marginUsed = 20000.0;
    input.marginLimit = 100000.0;
    input.buyingPowerUsed = 20000.0;
    input.buyingPowerLimit = 100000.0;
    input.riskUsed = 25000.0;
    input.riskLimit = 100000.0;
    input.orderQuantity = 10.0;
    input.orderedQuantity = 10.0;
    input.filledQuantity = 5.0;
    input.limitPrice = 99.5;
    input.stopPrice = 98.0;
    input.referencePrice = 100.0;
    input.alertValue = 100.0;
    input.alertThreshold = 101.0;
    input.momentumScore = 30.0;
    input.volatilityScore = 40.0;
    input.liquidityScore = 85.0;
    input.spreadQualityScore = 80.0;
    input.eventImpact = 40.0;
    input.minutesToEvent = 120.0;
    input.count = 10U;
    input.visibleCount = 8U;
    input.capacity = 100U;
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
    input.canPreviewOrder = 1;
    input.canSubmitOrder = 1;
    input.canCancelOrder = 1;
    input.revision = 77U;

    assert(umi_trading_professional_service_count() == 80U);
    for (index = 0U; index < umi_trading_professional_service_count(); ++index) {
        const char *id = NULL;
        assert(umi_trading_professional_service_id_at(index, &id) ==
               UMI_STATUS_OK);
        assert(id != NULL && id[0] != '\0');
        assert(umi_trading_professional_service_evaluate(
                   id, &input, &snapshot) == UMI_STATUS_OK);
        assert(strcmp(snapshot.id, id) == 0);
        assert(snapshot.revision == 77U);
    }

    assert(umi_trading_professional_service_evaluate(
               "missing.capability", &input, &snapshot) ==
           UMI_STATUS_NOT_FOUND);
    return 0;
}
