/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_resource_broker.c
 *
 * PURPOSE:
 *   Verify applications share scarce resources only through capability-checked
 *   Framework leases.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/application.h"

int main(void)
{
    UmiApplicationResourceBroker *broker = NULL;
    UmiResourceLeaseRequest request = {0};
    UmiResourceLease first;
    UmiResourceLease second;
    const UmiApplicationDefinition *studio =
        umi_application_portfolio_find("org.umicom.studio");
    const UmiApplicationDefinition *llm =
        umi_application_portfolio_find("org.umicom.llm");

    assert(umi_application_resource_broker_create(&broker) == UMI_STATUS_OK);
    request.application = studio;
    request.resource_id = "accelerator.gpu-0";
    request.capability_id = "umicom.ai";
    request.mode = UMI_RESOURCE_LEASE_SHARED_READ;
    request.quota_units = 4U;
    request.expires_at = 100U;
    assert(umi_application_resource_broker_acquire(
        broker, &request, &first) == UMI_STATUS_OK);
    request.application = llm;
    request.quota_units = 8U;
    assert(umi_application_resource_broker_acquire(
        broker, &request, &second) == UMI_STATUS_OK);
    assert(umi_application_resource_broker_active_count(broker) == 2U);

    request.mode = UMI_RESOURCE_LEASE_EXCLUSIVE;
    assert(umi_application_resource_broker_acquire(
        broker, &request, &second) == UMI_STATUS_BUSY);
    assert(umi_application_resource_broker_release(
        broker, studio->application_id, first.lease_id) == UMI_STATUS_OK);
    assert(umi_application_resource_broker_release(
        broker, llm->application_id, second.lease_id) == UMI_STATUS_OK);
    assert(umi_application_resource_broker_active_count(broker) == 0U);

    request.application = studio;
    request.resource_id = "broker.live";
    request.capability_id = "umicom.trading";
    request.mode = UMI_RESOURCE_LEASE_EXCLUSIVE;
    assert(umi_application_resource_broker_acquire(
        broker, &request, &first) == UMI_STATUS_PERMISSION_DENIED);
    umi_application_resource_broker_destroy(broker);
    return 0;
}
