/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/endpoint_projection.c
 *
 * PURPOSE:
 *   Build a context-aware panel endpoint inventory with queue and publication counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/endpoint_projection.h"
#include <string.h>
UmiStatus umi_workbench_context_host_endpoint_projection_build(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostEndpointProjection *out_projection)
{
    size_t i;if(!host||!out_projection)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_projection,0,sizeof(*out_projection));
    if(host->endpoints.count>UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINT_PROJECTION_ROWS)return UMI_STATUS_CAPACITY_EXCEEDED;
    for(i=0U;i<host->endpoints.count;++i){
        const UmiWorkbenchContextHostEndpoint *e=&host->endpoints.items[i];
        const UmiWorkbenchContextHostInbox *inbox=
            umi_workbench_context_host_inbox_registry_find_const(&host->inboxes,e->endpoint_id);
        UmiWorkbenchContextHostEndpointProjectionRow *r=&out_projection->rows[out_projection->count++];
        (void)umi_workbench_context_host_copy_text(r->endpoint_id,sizeof(r->endpoint_id),e->endpoint_id);
        (void)umi_workbench_context_host_copy_text(r->panel_id,sizeof(r->panel_id),e->panel_id);
        (void)umi_workbench_context_host_copy_text(r->display_name,sizeof(r->display_name),e->display_name);
        (void)umi_workbench_context_host_copy_text(r->group_id,sizeof(r->group_id),e->group_id);
        r->role=e->role;r->mode=e->mode;r->state=e->state;r->queued_delivery_count=inbox?inbox->count:0U;
        r->delivery_count=e->delivery_count;r->publish_count=e->publish_count;
    }
    out_projection->revision=host->revision;return UMI_STATUS_OK;
}
