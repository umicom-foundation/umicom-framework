/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_supervisor.c
 *
 * PURPOSE:
 *   Verify every application enters through the Framework Master Controller
 *   and receives federation, context and component authorities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/application/application.h"

int main(void)
{
    UmiApplicationSupervisor *supervisor = NULL;
    UmiApplicationSupervisorConfig config = {0};
    UmiCapabilityRegistry *capabilities;
    UmiFederationStats stats;

    config.application_id = "org.umicom.studio";
    assert(umi_application_supervisor_create(&config, &supervisor) ==
           UMI_STATUS_OK);
    assert(strcmp(umi_application_supervisor_definition(supervisor)->application_id,
                  config.application_id) == 0);
    capabilities = umi_master_controller_capabilities(
        umi_application_supervisor_master(supervisor));
    assert(umi_capability_registry_find(
        capabilities, "umicom.application.federation") != NULL);
    assert(umi_capability_registry_find(
        capabilities, "umicom.application.context") != NULL);
    assert(umi_capability_registry_find(
        capabilities, "umicom.application.resources") != NULL);
    assert(umi_application_supervisor_resources(supervisor) != NULL);
    stats = umi_federation_router_stats(
        umi_application_supervisor_federation(supervisor));
    assert(stats.application_count == 1U);
    assert(umi_application_supervisor_attach(
        supervisor, umi_application_portfolio_find("org.umicom.trader")) ==
        UMI_STATUS_OK);
    assert(umi_application_supervisor_start(supervisor) == UMI_STATUS_OK);
    assert(umi_application_supervisor_stop(supervisor) == UMI_STATUS_OK);
    umi_application_supervisor_destroy(supervisor);

    config.application_id = "org.umicom.unknown";
    assert(umi_application_supervisor_create(&config, &supervisor) ==
           UMI_STATUS_NOT_FOUND);
    return 0;
}
