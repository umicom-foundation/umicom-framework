/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/status_projection.c
 *
 * PURPOSE:
 *   Build concise active-link status including current context and queue pressure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/status_projection.h"
#include <stdio.h>
#include <string.h>
/*
 * Provide the workbench context host status projection build operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_status_projection_build(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostStatusProjection *out_projection)
{
    const UmiWorkbenchContextLinkGroupProfile *group=NULL;
    const UmiWorkbenchContextLinkActiveSlot *active=NULL;
    size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!host||!out_projection||!host->link_service)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_projection,0,sizeof(*out_projection));out_projection->suspended=host->suspended;
    (void)umi_workbench_context_host_copy_text(out_projection->active_group_id,
        sizeof(out_projection->active_group_id),host->active_group_id);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(host->active_group_id[0]){
        group=umi_workbench_context_link_group_catalogue_find_const(
            &host->link_service->groups,host->active_group_id);
        active=umi_workbench_context_link_service_current(host->link_service,host->active_group_id);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if(group){
        (void)umi_workbench_context_host_copy_text(out_projection->active_group_title,
            sizeof(out_projection->active_group_title),group->title);
        out_projection->colour=group->colour;
        /* Visit each bounded item once so every record receives the same rule. */
        for(i=0U;i<host->endpoints.count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(host->endpoints.items[i].group_id,group->group_id)==0)
            ++out_projection->linked_endpoint_count;
    }
    /* Apply this operation only while the related capability or state is available. */
    if(active){
        out_projection->has_context=true;out_projection->context_kind=active->payload.kind;
        (void)umi_workbench_context_host_copy_text(out_projection->active_context_id,
            sizeof(out_projection->active_context_id),active->payload.identity.context_id);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<host->inboxes.count;++i)out_projection->queued_delivery_count+=host->inboxes.items[i].count;
    (void)snprintf(out_projection->summary,sizeof(out_projection->summary),
        "%s%s%s · %zu linked · %zu queued",
        out_projection->active_group_title[0]?out_projection->active_group_title:"Unlinked",
        active?" · ":"",active?umi_context_kind_text(active->payload.kind):"",
        out_projection->linked_endpoint_count,out_projection->queued_delivery_count);
    out_projection->revision=host->revision;return UMI_STATUS_OK;
}
