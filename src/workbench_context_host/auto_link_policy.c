/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/auto_link_policy.c
 *
 * PURPOSE:
 *   Select compatible configured groups using endpoint precedent and context-kind compatibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/auto_link_policy.h"
#include <string.h>
/*
 * Provide the workbench context host auto link decide operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_auto_link_decide(
    const UmiWorkbenchContextHostProfile *profile,UmiWorkbenchContextHostPanelRole role,
    UmiContextKind kind,UmiWorkbenchContextHostAutoLinkDecision *out_decision)
{
    size_t i,j;int best=-1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!profile||!out_decision)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_decision,0,sizeof(*out_decision));out_decision->mode=UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<profile->group_count;++i){
        const UmiWorkbenchContextHostGroupDefinition *g=&profile->groups[i];int score=0;
        /* Apply this operation only while the related capability or state is available. */
        if(!umi_workbench_context_host_kind_allowed(g->allowed_kinds_mask,kind))continue;
        /* Apply this operation only while the related capability or state is available. */
        if(g->default_active)score+=20;
        /* Visit each bounded item once so every record receives the same rule. */
        for(j=0U;j<profile->endpoint_count;++j){
            const UmiWorkbenchContextHostEndpoint *e=&profile->endpoints[j];
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if(strcmp(e->group_id,g->group_id)==0){
                /* Apply this branch only when its contract condition is satisfied. */
                if(e->role==role)score+=100;
                /* Apply this branch only when its contract condition is satisfied. */
                if(umi_workbench_context_host_endpoint_accepts(e,kind))score+=10;
                /* Apply this branch only when its contract condition is satisfied. */
                if(umi_workbench_context_host_endpoint_publishes(e,kind))score+=10;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if(score>best){best=score;out_decision->found=true;out_decision->confidence=score;
            out_decision->mode=g->default_mode;(void)umi_workbench_context_host_copy_text(
                out_decision->group_id,sizeof(out_decision->group_id),g->group_id);}
    }
    return UMI_STATUS_OK;
}
