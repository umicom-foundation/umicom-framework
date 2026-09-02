/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/snapshot.c
 *
 * PURPOSE:
 *   Build immutable provider inventory, history and metrics snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/snapshot.h"

#include <string.h>

/*
 * Provide the workbench selection provider snapshot build operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_snapshot_build(
    const UmiWorkbenchSelectionProviderService *service,
    UmiWorkbenchSelectionProviderSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->provider_count = service->registry.count;
    out_snapshot->history_count = service->history.count;
    out_snapshot->metrics = service->metrics;
    out_snapshot->registry_revision = service->registry.revision;
    out_snapshot->history_revision = service->history.revision;
    out_snapshot->revision = service->revision;
    out_snapshot->suspended = service->suspended;
    return UMI_STATUS_OK;
}
