/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/route_preview.c
 *
 * PURPOSE:
 *   Build recipient previews from the same endpoint/group constraints used by live routing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/route_preview.h"
#include <string.h>
UmiStatus umi_workbench_context_host_route_preview_build(
    const UmiWorkbenchContextHost *host,const char *source_endpoint_id,const char *group_id,
    UmiContextKind kind,UmiWorkbenchContextHostRoutePreview *out_preview)
{
    size_t i;if(!host||!source_endpoint_id||!group_id||!out_preview)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_preview,0,sizeof(*out_preview));
    for(i=0U;i<host->endpoints.count&&out_preview->count<UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS;++i){
        const UmiWorkbenchContextHostEndpoint *e=&host->endpoints.items[i];
        UmiWorkbenchContextHostCompatibility c;UmiWorkbenchContextHostRoutePreviewRow *r;
        if(strcmp(e->endpoint_id,source_endpoint_id)==0)continue;
        if(umi_workbench_context_host_compatibility_evaluate(host,source_endpoint_id,e->endpoint_id,group_id,kind,&c)!=UMI_STATUS_OK)continue;
        r=&out_preview->rows[out_preview->count++];(void)umi_workbench_context_host_copy_text(r->endpoint_id,sizeof(r->endpoint_id),e->endpoint_id);
        (void)umi_workbench_context_host_copy_text(r->panel_id,sizeof(r->panel_id),e->panel_id);
        (void)umi_workbench_context_host_copy_text(r->application_id,sizeof(r->application_id),e->application_id);
        r->compatibility_score=c.score;r->will_receive=c.compatible;if(r->will_receive)++out_preview->recipient_count;
    }
    out_preview->revision=host->revision;return UMI_STATUS_OK;
}
