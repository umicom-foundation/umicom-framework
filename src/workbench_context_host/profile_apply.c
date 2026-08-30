/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/profile_apply.c
 *
 * PURPOSE:
 *   Apply group definitions and endpoint bindings without product-specific routing logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/profile_apply.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_workbench_context_host_profile_apply(
    const UmiWorkbenchContextHostProfile *profile,
    UmiWorkbenchContextLinkService *link_service,
    UmiWorkbenchContextHostEndpointRegistry *endpoints,
    char *out_default_group,size_t out_default_group_capacity)
{
    size_t i;UmiStatus s;const char *default_group;
    if(!profile||!link_service||!endpoints||!out_default_group||out_default_group_capacity==0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_host_profile_validate(profile);if(s!=UMI_STATUS_OK)return s;
    for(i=0U;i<profile->group_count;++i){
        const UmiWorkbenchContextHostGroupDefinition *g=&profile->groups[i];
        s=umi_workbench_context_link_service_define_group(
            link_service,g->group_id,g->title,g->colour,g->allowed_kinds_mask,g->default_mode);
        if(s!=UMI_STATUS_OK)return s;
    }
    for(i=0U;i<profile->endpoint_count;++i){
        const UmiWorkbenchContextHostEndpoint *e=&profile->endpoints[i];
        char binding_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
        int written=snprintf(binding_id,sizeof(binding_id),"%s.binding",e->endpoint_id);
        if(written<0||(size_t)written>=sizeof(binding_id))return UMI_STATUS_CAPACITY_EXCEEDED;
        s=umi_workbench_context_link_service_bind_panel(
            link_service,binding_id,e->panel_id,e->application_id,e->group_id,
            UMI_CONTEXT_KIND_GENERIC,e->mode);
        if(s!=UMI_STATUS_OK)return s;
        s=umi_workbench_context_host_endpoint_registry_upsert(endpoints,e);
        if(s!=UMI_STATUS_OK)return s;
    }
    default_group=umi_workbench_context_host_profile_default_group(profile);
    if(!default_group)return UMI_STATUS_NOT_FOUND;
    return umi_workbench_context_host_copy_text(
        out_default_group,out_default_group_capacity,default_group);
}
