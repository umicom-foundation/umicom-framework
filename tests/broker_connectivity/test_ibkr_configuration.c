/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_ibkr_configuration.c
 *
 * PURPOSE:
 *   Verify IBKR Configuration broker-connectivity behaviour.
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

#include "umicom/broker_connectivity/ibkr/configuration.h"

int main(void)
{
    UmiBrokerConnectivityInput input;
    UmiBrokerConnectivitySnapshot snapshot;

    umi_broker_connectivity_input_init(&input);
    input.value = 50.0;
    input.secondaryValue = 25.0;
    input.limitValue = 100.0;
    input.netLiquidation = 100000.0;
    input.cash = 25000.0;
    input.buyingPower = 100000.0;
    input.marginUsed = 20000.0;
    input.marginLimit = 100000.0;
    input.dailyPnl = 250.0;
    input.unrealisedPnl = 100.0;
    input.realisedPnl = 150.0;
    input.orderQuantity = 10.0;
    input.orderPrice = 100.0;
    input.orderNotional = 1000.0;
    input.workingOrderNotional = 2000.0;
    input.positionQuantity = 20.0;
    input.maxOrderQuantity = 100.0;
    input.maxOrderNotional = 10000.0;
    input.maxPositionQuantity = 100.0;
    input.maxDailyLoss = 1000.0;
    input.grossExposure = 25000.0;
    input.netExposure = 10000.0;
    input.concentrationPercent = 20.0;
    input.leveragePercent = 25.0;
    input.priceDeviationPercent = 2.0;
    input.orderedQuantity = 10.0;
    input.filledQuantity = 10.0;
    input.averageFillPrice = 100.05;
    input.referencePrice = 100.0;
    input.commission = 1.0;
    input.slippage = 0.05;
    input.ageMilliseconds = 100U;
    input.latencyMilliseconds = 25U;
    input.reconnectAttempts = 0U;
    input.mappedCount = 10U;
    input.totalCount = 10U;
    input.matchedCount = 10U;
    input.mismatchCount = 0U;
    input.duplicateCount = 0U;
    input.lateCount = 0U;
    input.correctionCount = 0U;
    input.rejectionCount = 0U;
    input.auditCount = 10U;
    input.persistedAuditCount = 10U;
    input.gapCount = 0U;
    input.connected = 1;
    input.authenticated = 1;
    input.providerReady = 1;
    input.accountReady = 1;
    input.orderReady = 1;
    input.executionReady = 1;
    input.positionReady = 1;
    input.riskReady = 1;
    input.auditReady = 1;
    input.paperCapable = 1;
    input.liveCapable = 1;
    input.liveApproved = 1;
    input.killSwitchEngaged = 0;
    input.restrictedInstrument = 0;
    input.trusted = 1;
    input.revision = 123U;

    assert(umi_broker_connectivity_evaluate_ibkr_configuration(&input, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.id, "ibkr.configuration") == 0);
    assert(snapshot.revision == 123U);
    assert(snapshot.score >= 0.0 && snapshot.score <= 100.0);
    assert(snapshot.blocked == 0);
    assert(snapshot.ready == 1);
    return 0;
}
