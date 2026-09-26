/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_runtime_bridge.c
 *
 * PURPOSE:
 *   Exercise real catalogue-to-federation projection and stale-instance rejection.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "test_runtime_fixture.h"
#include <stdlib.h>
int main(void)
{
    UmiDeskRuntime *runtime = NULL;
    UmiDeskFederationPorts ports;
    UmiDeskFederationCatalogue *copy = calloc(1U, sizeof *copy);
    UmiApplicationRuntimeCatalogue *catalogue;
    UmiDataServer *server = NULL;
    UmiDeskFederation *service = NULL;
    UmiDeskWorkspace workspace;
    UmiDeskFederationPlan plan;
    UmiApplicationRuntimeRecord record;
    size_t calls = 0U;
    REQUIRE(copy != NULL);
    REQUIRE(FixtureRuntime(&calls, &runtime) == UMI_STATUS_OK);
    catalogue = umi_desk_runtime_applications(runtime);
    ports = UmiDeskFederationRuntimePorts(runtime);
    REQUIRE(ports.read(ports.context, copy) == UMI_STATUS_OK);
    REQUIRE(copy->count == umi_application_runtime_catalogue_count(catalogue));
    REQUIRE(copy->count > 0U && copy->count <= UMI_DESK_FEDERATION_MAX_APPS);
    for (size_t index = 0U; index < copy->count; ++index) {
        REQUIRE(umi_application_runtime_catalogue_at(catalogue, index, &record) == UMI_STATUS_OK);
        REQUIRE(strcmp(copy->apps[index].id.value, record.application_id) == 0);
        REQUIRE(strcmp(copy->apps[index].title, record.display_name) == 0);
        REQUIRE(copy->apps[index].installed == record.installed);
        REQUIRE(copy->apps[index].enabled == record.enabled);
        REQUIRE(copy->apps[index].running == record.running);
        REQUIRE(copy->apps[index].processToken == record.process_token);
    }
    /* Changing a canonical observation invalidates the old expected record,
     * before the bridge can ask the existing launcher to do any work. */
    UmiDeskFederationApp expected = copy->apps[0];
    REQUIRE(umi_application_runtime_catalogue_set_presence(catalogue, expected.id.value,
        !expected.installed, expected.compatible, expected.enabled) == UMI_STATUS_OK);
    REQUIRE(ports.request(ports.context, &expected, UMI_DESK_FEDERATION_START) == UMI_STATUS_BUSY);
    REQUIRE(calls == 0U);
    REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    REQUIRE(UmiDeskFederationCreate(server, &ports, "org.umicom.desktop", &service) == UMI_STATUS_OK);
    REQUIRE(UmiDeskFederationReload(service) == UMI_STATUS_OK);
    REQUIRE(UmiDeskFederationSaveSelection(service, runtime, "empty", "Empty selection") == UMI_STATUS_INVALID_STATE);
    REQUIRE(UmiDeskFederationWorkspaceAt(service, 0U, &workspace) == UMI_STATUS_NOT_FOUND);
    REQUIRE(UmiDeskFederationPrepareApplication(service, "org.umicom.desktop", &plan) == UMI_STATUS_OK);
    REQUIRE(plan.count == 1U && plan.items[0].action == UMI_DESK_FEDERATION_SKIP);
    REQUIRE(calls == 0U);
    UmiDeskFederationDestroy(service); umi_data_server_destroy(server);
    umi_desk_runtime_destroy(runtime); free(copy);
    puts("PASS actual Desk runtime projection and stale-request guard; no processes launched");
    return 0;
}
