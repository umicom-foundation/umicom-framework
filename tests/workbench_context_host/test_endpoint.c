/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_endpoint.c
 *
 * PURPOSE:
 *   Verify endpoint identity, group assignment and kind capabilities.
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
#include "test_support.h"

int main(void)
{

    UmiWorkbenchContextHostEndpoint endpoint;
    umi_workbench_context_host_endpoint_init(&endpoint, "endpoint");
    assert(umi_workbench_context_host_endpoint_set_identity(
        &endpoint, "panel", "application", "Panel") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_endpoint_set_group(
        &endpoint, "blue",
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) == UMI_STATUS_OK);
    endpoint.role = UMI_WORKBENCH_CONTEXT_HOST_PANEL_EDITOR;
    endpoint.accepted_kinds_mask =
        umi_workbench_context_host_kind_mask(UMI_CONTEXT_KIND_SOURCE_LOCATION);
    endpoint.published_kinds_mask =
        umi_workbench_context_host_kind_mask(UMI_CONTEXT_KIND_SELECTION);
    assert(umi_workbench_context_host_endpoint_validate(&endpoint) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_endpoint_accepts(
        &endpoint, UMI_CONTEXT_KIND_SOURCE_LOCATION));
    assert(!umi_workbench_context_host_endpoint_accepts(
        &endpoint, UMI_CONTEXT_KIND_TRADE));
    assert(umi_workbench_context_host_endpoint_publishes(
        &endpoint, UMI_CONTEXT_KIND_SELECTION));
    return 0;
}
