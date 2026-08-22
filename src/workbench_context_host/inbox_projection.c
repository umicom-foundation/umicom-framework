/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/inbox_projection.c
 *
 * PURPOSE:
 *   Build per-endpoint pending-delivery and drop statistics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/inbox_projection.h"
#include <string.h>
UmiStatus umi_workbench_context_host_inbox_projection_build(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostInboxProjection *out_projection)
{
    size_t i;if(!host||!out_projection)return UMI_STATUS_INVALID_ARGUMENT;memset(out_projection,0,sizeof(*out_projection));
    if(host->inboxes.count>UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS)return UMI_STATUS_CAPACITY_EXCEEDED;
    for(i=0U;i<host->inboxes.count;++i){
        const UmiWorkbenchContextHostInbox *inbox=&host->inboxes.items[i];
        const UmiWorkbenchContextHostDelivery *next=umi_workbench_context_host_inbox_peek(inbox);
        UmiWorkbenchContextHostInboxProjectionRow *row=&out_projection->rows[out_projection->count++];
        (void)umi_workbench_context_host_copy_text(row->endpoint_id,sizeof(row->endpoint_id),inbox->endpoint_id);
        (void)umi_workbench_context_host_copy_text(row->panel_id,sizeof(row->panel_id),inbox->panel_id);
        row->queued_count=inbox->count;row->dropped_count=inbox->dropped_count;
        out_projection->total_queued+=inbox->count;out_projection->total_dropped+=inbox->dropped_count;
        if(next){(void)umi_workbench_context_host_copy_text(row->next_context_id,sizeof(row->next_context_id),next->payload.identity.context_id);
            row->next_kind=next->payload.kind;}
    }
    out_projection->revision=host->revision;return UMI_STATUS_OK;
}
