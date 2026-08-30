/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/group_picker_projection.c
 *
 * PURPOSE:
 *   Build a deterministic group picker from canonical group and endpoint state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/group_picker_projection.h"
#include <string.h>
UmiStatus umi_workbench_context_host_group_picker_projection_build(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostGroupPickerProjection *out_projection)
{
    size_t i,j;if(!host||!out_projection||!host->link_service)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_projection,0,sizeof(*out_projection));
    if(host->link_service->groups.count>UMI_WORKBENCH_CONTEXT_HOST_MAX_GROUPS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for(i=0U;i<host->link_service->groups.count;++i){
        const UmiWorkbenchContextLinkGroupProfile *g=&host->link_service->groups.items[i];
        UmiWorkbenchContextHostGroupPickerRow *row=&out_projection->rows[out_projection->count++];
        (void)umi_workbench_context_host_copy_text(row->group_id,sizeof(row->group_id),g->group_id);
        (void)umi_workbench_context_host_copy_text(row->title,sizeof(row->title),g->title);
        row->colour=g->colour;row->active=strcmp(host->active_group_id,g->group_id)==0;
        for(j=0U;j<host->endpoints.count;++j)if(strcmp(host->endpoints.items[j].group_id,g->group_id)==0)
            ++row->member_count;
    }
    out_projection->revision=host->revision;return UMI_STATUS_OK;
}
