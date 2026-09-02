/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_snapshot.c
 *
 * PURPOSE:
 *   Verify immutable service snapshot counts and metrics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/workbench_context_link/snapshot.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextLinkService service;
    UmiWorkbenchContextLinkSnapshot snapshot;
    umi_workbench_context_link_service_init(&service);
    assert(umi_workbench_context_link_service_define_group(
        &service, "red", "Red", UMI_CONTEXT_COLOUR_RED,
        UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_snapshot_build(&service, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.group_count == 1U);
    assert(snapshot.revision == service.revision);
    umi_workbench_context_link_service_destroy(&service);
    return 0;
}
