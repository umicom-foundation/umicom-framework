/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/history_projection.c
 *
 * PURPOSE:
 *   Build a bounded reverse-chronological history view optionally filtered by group.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/history_projection.h"
#include <string.h>
/*
 * Provide the workbench context host history projection build operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_history_projection_build(
    const UmiWorkbenchContextHost *host,const char *group_id,
    UmiWorkbenchContextHostHistoryProjection *out_projection)
{
    size_t index;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!host||!out_projection||!host->link_service)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_projection,0,sizeof(*out_projection));index=host->link_service->history.count;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while(index>0U&&out_projection->count<UMI_WORKBENCH_CONTEXT_HOST_HISTORY_PROJECTION_LIMIT){
        const UmiWorkbenchContextLinkHistoryItem *item=&host->link_service->history.items[--index];
        UmiWorkbenchContextHostHistoryProjectionRow *row;/* Use the stable identifier comparison to choose the matching record or policy. */ if(group_id&&group_id[0]&&strcmp(group_id,item->group_id)!=0)continue;
        row=&out_projection->rows[out_projection->count++];
        (void)umi_workbench_context_host_copy_text(row->group_id,sizeof(row->group_id),item->group_id);
        (void)umi_workbench_context_host_copy_text(row->context_id,sizeof(row->context_id),item->payload.identity.context_id);
        (void)umi_workbench_context_host_copy_text(row->source_application_id,sizeof(row->source_application_id),item->payload.identity.source_application_id);
        (void)umi_workbench_context_host_copy_text(row->source_panel_id,sizeof(row->source_panel_id),item->payload.identity.source_panel_id);
        row->kind=item->payload.kind;row->sequence=item->sequence;row->published_at_ms=item->published_at_ms;
    }
    out_projection->revision=host->revision;return UMI_STATUS_OK;
}
