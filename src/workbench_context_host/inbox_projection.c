/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/inbox_projection.c
 *
 * PURPOSE:
 *   Build per-endpoint pending-delivery and drop statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/inbox_projection.h"
#include <string.h>
/*
 * Provide the workbench context host inbox projection build operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_inbox_projection_build(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostInboxProjection *out_projection)
{
    size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!host||!out_projection)return UMI_STATUS_INVALID_ARGUMENT;memset(out_projection,0,sizeof(*out_projection));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(host->inboxes.count>UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS)return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<host->inboxes.count;++i){
        const UmiWorkbenchContextHostInbox *inbox=&host->inboxes.items[i];
        const UmiWorkbenchContextHostDelivery *next=umi_workbench_context_host_inbox_peek(inbox);
        UmiWorkbenchContextHostInboxProjectionRow *row=&out_projection->rows[out_projection->count++];
        (void)umi_workbench_context_host_copy_text(row->endpoint_id,sizeof(row->endpoint_id),inbox->endpoint_id);
        (void)umi_workbench_context_host_copy_text(row->panel_id,sizeof(row->panel_id),inbox->panel_id);
        row->queued_count=inbox->count;row->dropped_count=inbox->dropped_count;
        out_projection->total_queued+=inbox->count;out_projection->total_dropped+=inbox->dropped_count;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if(next){(void)umi_workbench_context_host_copy_text(row->next_context_id,sizeof(row->next_context_id),next->payload.identity.context_id);
            row->next_kind=next->payload.kind;}
    }
    out_projection->revision=host->revision;return UMI_STATUS_OK;
}
