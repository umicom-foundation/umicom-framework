/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_endpoint_registry.c
 *
 * PURPOSE:
 *   Verify endpoint registry replacement, lookup and removal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "test_support.h"

int main(void)
{

    UmiWorkbenchContextHostEndpointRegistry registry;
    UmiWorkbenchContextHostEndpoint endpoint;
    umi_workbench_context_host_endpoint_registry_init(&registry);
    umi_workbench_context_host_endpoint_init(&endpoint, "endpoint");
    assert(umi_workbench_context_host_endpoint_set_identity(
        &endpoint, "panel", "application", "Panel") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_endpoint_set_group(
        &endpoint, "blue", UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_endpoint_registry_upsert(
        &registry, &endpoint) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_endpoint_registry_count(&registry) == 1U);
    assert(umi_workbench_context_host_endpoint_registry_find_panel(
        &registry, "panel") != NULL);
    endpoint.state = UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ACTIVE;
    assert(umi_workbench_context_host_endpoint_registry_upsert(
        &registry, &endpoint) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_endpoint_registry_count(&registry) == 1U);
    assert(umi_workbench_context_host_endpoint_registry_remove(
        &registry, "endpoint") == UMI_STATUS_OK);
    umi_workbench_context_host_endpoint_registry_destroy(&registry);
    return 0;
}
