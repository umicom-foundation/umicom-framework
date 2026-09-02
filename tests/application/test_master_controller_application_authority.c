/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_master_controller_application_authority.c
 *
 * PURPOSE:
 *   Verify the established Master Controller owns the application portfolio,
 *   federation, context and resource authorities and continues to control the
 *   complete Slave Controller lifecycle directly.
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

#include "umicom/application/application.h"

static int authority_destroyed = 0;

/*
 * Exercise mark authority destroyed and return a clear result when the behaviour no longer
 * matches its contract.
 */
static void mark_authority_destroyed(void *authority)
{
    assert(authority != NULL);
    authority_destroyed += 1;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiMasterController *master = NULL;
    UmiMasterControllerConfig config = {0};
    UmiCapabilityRegistry *capabilities;
    UmiFederationStats stats;
    int test_authority = 1;

    config.application_name = "Umicom Studio IDE";
    assert(umi_master_controller_create(&config, &master) == UMI_STATUS_OK);
    assert(umi_master_controller_install_application_authority(
               master, "org.umicom.studio") == UMI_STATUS_OK);
    assert(strcmp(umi_master_controller_application_name(master),
                  "Umicom Studio IDE") == 0);
    assert(strcmp(umi_master_controller_application_definition(master)
                      ->application_id,
                  "org.umicom.studio") == 0);

    capabilities = umi_master_controller_capabilities(master);
    assert(umi_capability_registry_find(
               capabilities, "umicom.application.federation") != NULL);
    assert(umi_capability_registry_find(
               capabilities, "umicom.application.context") != NULL);
    assert(umi_capability_registry_find(
               capabilities, "umicom.application.resources") != NULL);
    assert(umi_master_controller_application_resources(master) != NULL);

    stats = umi_federation_router_stats(
        umi_master_controller_application_federation(master));
    assert(stats.application_count == 1U);
    assert(umi_master_controller_attach_application(
               master,
               umi_application_portfolio_find("org.umicom.trader")) ==
           UMI_STATUS_OK);
    assert(umi_master_controller_register_authority(
               master, "org.umicom.test.authority", &test_authority,
               mark_authority_destroyed) == UMI_STATUS_OK);
    assert(umi_master_controller_start(master) == UMI_STATUS_OK);
    assert(umi_master_controller_stop(master) == UMI_STATUS_OK);
    umi_master_controller_destroy(master);
    assert(authority_destroyed == 1);

    assert(umi_master_controller_create(&config, &master) == UMI_STATUS_OK);
    assert(umi_master_controller_install_application_authority(
               master, "org.umicom.unknown") == UMI_STATUS_NOT_FOUND);
    umi_master_controller_destroy(master);
    return 0;
}
