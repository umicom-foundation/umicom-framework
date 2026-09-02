/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/compatibility.c
 *
 * PURPOSE:
 *   Evaluate endpoint mode, type-mask and group policy compatibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/compatibility.h"
#include <string.h>
/*
 * Provide the workbench context host compatibility evaluate operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_compatibility_evaluate(
    const UmiWorkbenchContextHost *host,const char *source_endpoint_id,
    const char *target_endpoint_id,const char *group_id,UmiContextKind kind,
    UmiWorkbenchContextHostCompatibility *out)
{
    const UmiWorkbenchContextHostEndpoint *source,*target;
    const UmiWorkbenchContextLinkGroupProfile *group;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!host||!source_endpoint_id||!target_endpoint_id||!group_id||!out)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));source=umi_workbench_context_host_endpoint_registry_find_const(&host->endpoints,source_endpoint_id);
    target=umi_workbench_context_host_endpoint_registry_find_const(&host->endpoints,target_endpoint_id);
    group=umi_workbench_context_link_group_catalogue_find_const(&host->link_service->groups,group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!source||!target||!group)return UMI_STATUS_NOT_FOUND;
    out->source_can_publish=source->mode==UMI_WORKBENCH_CONTEXT_LINK_MODE_PUBLISH||
        source->mode==UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    out->group_accepts=umi_workbench_context_host_kind_allowed(group->flags,kind);
    out->target_can_follow=target->mode==UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW||
        target->mode==UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    out->target_accepts=umi_workbench_context_host_endpoint_accepts(target,kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(strcmp(source->group_id,group_id)!=0||strcmp(target->group_id,group_id)!=0){out->score=0;return UMI_STATUS_OK;}
    out->score=(out->source_can_publish?25:0)+(out->group_accepts?25:0)+(out->target_can_follow?25:0)+(out->target_accepts?25:0);
    out->compatible=out->score==100;return UMI_STATUS_OK;
}
