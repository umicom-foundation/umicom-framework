/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/auto_link_policy.c
 *
 * PURPOSE:
 *   Select compatible configured groups using endpoint precedent and context-kind compatibility.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/auto_link_policy.h"
#include <string.h>
UmiStatus umi_workbench_context_host_auto_link_decide(
    const UmiWorkbenchContextHostProfile *profile,UmiWorkbenchContextHostPanelRole role,
    UmiContextKind kind,UmiWorkbenchContextHostAutoLinkDecision *out_decision)
{
    size_t i,j;int best=-1;if(!profile||!out_decision)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_decision,0,sizeof(*out_decision));out_decision->mode=UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    for(i=0U;i<profile->group_count;++i){
        const UmiWorkbenchContextHostGroupDefinition *g=&profile->groups[i];int score=0;
        if(!umi_workbench_context_host_kind_allowed(g->allowed_kinds_mask,kind))continue;
        if(g->default_active)score+=20;
        for(j=0U;j<profile->endpoint_count;++j){
            const UmiWorkbenchContextHostEndpoint *e=&profile->endpoints[j];
            if(strcmp(e->group_id,g->group_id)==0){
                if(e->role==role)score+=100;
                if(umi_workbench_context_host_endpoint_accepts(e,kind))score+=10;
                if(umi_workbench_context_host_endpoint_publishes(e,kind))score+=10;
            }
        }
        if(score>best){best=score;out_decision->found=true;out_decision->confidence=score;
            out_decision->mode=g->default_mode;(void)umi_workbench_context_host_copy_text(
                out_decision->group_id,sizeof(out_decision->group_id),g->group_id);}
    }
    return UMI_STATUS_OK;
}
