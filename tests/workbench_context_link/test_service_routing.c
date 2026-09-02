/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_service_routing.c
 *
 * PURPOSE:
 *   Verify typed publication routes only to compatible followers in the same colour group.
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
#include "umicom/workbench_context_link/payload_builder.h"
#include "umicom/workbench_context_link/service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextLinkService service;
    UmiContextPayload payload;
    UmiWorkbenchContextLinkDeliveryBatch deliveries;
    const uint64_t mask = umi_workbench_context_link_kind_mask(UMI_CONTEXT_KIND_PROJECT);
    umi_workbench_context_link_service_init(&service);
    assert(umi_workbench_context_link_service_define_group(
        &service, "blue", "Blue", UMI_CONTEXT_COLOUR_BLUE, mask,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_service_bind_panel(
        &service, "explorer-binding", "project-explorer", "studio", "blue",
        UMI_CONTEXT_KIND_PROJECT, UMI_WORKBENCH_CONTEXT_LINK_MODE_PUBLISH) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_service_bind_panel(
        &service, "editor-binding", "editor", "studio", "blue",
        UMI_CONTEXT_KIND_PROJECT, UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_build_project(
        &payload, "project-context", "studio", "project-explorer",
        "project-1", "C:/dev/project", "c") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_service_publish(
        &service, "blue", "project-explorer", &payload, 10U, &deliveries) == UMI_STATUS_OK);
    assert(deliveries.count == 1U);
    assert(strcmp(deliveries.items[0].panel_id, "editor") == 0);
    assert(service.metrics.publish_count == 1U);
    assert(service.metrics.delivery_count == 1U);
    umi_workbench_context_link_service_destroy(&service);
    return 0;
}
