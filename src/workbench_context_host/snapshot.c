/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/snapshot.c
 *
 * PURPOSE:
 *   Build immutable context-host snapshots without leaking mutable registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/snapshot.h"
#include <string.h>
UmiStatus umi_workbench_context_host_snapshot(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostSnapshot *out_snapshot)
{
    size_t i;if(!host||!out_snapshot)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot,0,sizeof(*out_snapshot));out_snapshot->structure_size=(uint32_t)sizeof(*out_snapshot);
    (void)umi_workbench_context_host_copy_text(out_snapshot->host_id,sizeof(out_snapshot->host_id),host->host_id);
    (void)umi_workbench_context_host_copy_text(out_snapshot->application_id,sizeof(out_snapshot->application_id),host->application_id);
    (void)umi_workbench_context_host_copy_text(out_snapshot->active_group_id,sizeof(out_snapshot->active_group_id),host->active_group_id);
    out_snapshot->endpoint_count=host->endpoints.count;out_snapshot->inbox_count=host->inboxes.count;
    for(i=0U;i<host->inboxes.count;++i)out_snapshot->queued_delivery_count+=host->inboxes.items[i].count;
    out_snapshot->group_count=host->link_service?host->link_service->groups.count:0U;
    out_snapshot->metrics=host->metrics;out_snapshot->suspended=host->suspended;
    out_snapshot->link_revision=host->link_service?host->link_service->revision:0U;out_snapshot->revision=host->revision;
    return UMI_STATUS_OK;
}
