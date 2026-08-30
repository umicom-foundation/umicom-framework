/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/snapshot.c
 *
 * PURPOSE:
 *   Build immutable source-service state without exposing mutable runtime arrays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/snapshot.h"

#include <string.h>

UmiStatus umi_workbench_context_source_snapshot_build(
    const UmiWorkbenchContextSourceService *service,
    UmiWorkbenchContextSourceSnapshot *out_snapshot)
{
    size_t index;
    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->source_count = service->registry.count;
    out_snapshot->runtime_count = service->runtime_count;
    for (index = 0U; index < service->registry.count; ++index) {
        if (service->registry.items[index].enabled) {
            ++out_snapshot->active_source_count;
        }
    }
    out_snapshot->metrics = service->metrics;
    out_snapshot->queued_event_count =
        service->events != NULL
        ? (uint64_t)service->events->queue.count
        : 0U;
    out_snapshot->revision = service->revision;
    out_snapshot->suspended = service->suspended;
    return UMI_STATUS_OK;
}
