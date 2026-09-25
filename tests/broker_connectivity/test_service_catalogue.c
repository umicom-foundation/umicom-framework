/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_service_catalogue.c
 *
 * PURPOSE:
 *   Verify the complete broker-connectivity policy catalogue is discoverable.
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
#include "umicom/broker_connectivity/service.h"

int main(void)
{
    UmiBrokerConnectivityInput input;
    UmiBrokerConnectivitySnapshot snapshot;
    size_t index;

    umi_broker_connectivity_input_init(&input);
    input.value = 50.0;
    input.limitValue = 100.0;
    input.marginUsed = 20.0;
    input.marginLimit = 100.0;
    input.dailyPnl = 100.0;
    input.maxDailyLoss = 1000.0;
    input.orderNotional = 1000.0;
    input.buyingPower = 10000.0;
    input.orderedQuantity = 10.0;
    input.filledQuantity = 10.0;
    input.averageFillPrice = 100.05;
    input.referencePrice = 100.0;
    input.ageMilliseconds = 100U;
    input.latencyMilliseconds = 25U;
    input.mappedCount = 10U;
    input.totalCount = 10U;
    input.matchedCount = 10U;
    input.auditCount = 10U;
    input.persistedAuditCount = 10U;
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
    input.trusted = 1;
    input.revision = 77U;

    assert(umi_broker_connectivity_service_count() == 80U);
    for (index = 0U; index < umi_broker_connectivity_service_count(); ++index) {
        const char *id = NULL;
        assert(umi_broker_connectivity_service_id_at(index, &id) ==
               UMI_STATUS_OK);
        assert(id != NULL && id[0] != '\0');
        assert(umi_broker_connectivity_service_evaluate(
                   id, &input, &snapshot) == UMI_STATUS_OK);
        assert(strcmp(snapshot.id, id) == 0);
        assert(snapshot.revision == 77U);
    }
    return 0;
}
