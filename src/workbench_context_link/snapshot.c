/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/snapshot.c
 *
 * PURPOSE:
 *   Build immutable workbench context-link snapshots for frontend rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/snapshot.h"

#include <string.h>

/*
 * Provide the workbench context link snapshot build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_link_snapshot_build(
    const UmiWorkbenchContextLinkService *service,
    UmiWorkbenchContextLinkSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->group_count = service->groups.count;
    out_snapshot->binding_count = service->bindings.count;
    out_snapshot->active_context_count = service->active.count;
    out_snapshot->history_count = service->history.count;
    out_snapshot->pin_count = service->pins.count;
    out_snapshot->metrics = service->metrics;
    out_snapshot->suspended = service->suspended;
    out_snapshot->revision = service->revision;
    return UMI_STATUS_OK;
}
