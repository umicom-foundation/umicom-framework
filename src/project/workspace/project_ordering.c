/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/project_ordering.c
 *
 * PURPOSE:
 *   Implement the project ordering behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_ordering.h"
/*
 * Provide the project workspace project ordering topological operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_project_ordering_topological(const UmiProjectWorkspaceProjectGraph*g,size_t*out_order,size_t capacity,size_t*out_count) {
    size_t indegree[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY]= {
        0U
    };
    bool emitted[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY]= {
        false
    };
    size_t i,j,n=0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(g==NULL||out_count==NULL||(g->count>0U&&out_order==NULL))return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(capacity<g->count)return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<g->count;++i)/* Visit each bounded item once so every record receives the same rule. */ for(j=0U;j<g->count;++j)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(g->edges[i][j])indegree[j]+=1U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while(n<g->count) {
        size_t pick=SIZE_MAX;
        /* Visit each bounded item once so every record receives the same rule. */
        for(i=0U;i<g->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!emitted[i]&&indegree[i]==0U) {
            pick=i;
            break;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if(pick==SIZE_MAX)return UMI_STATUS_INVALID_STATE;
        emitted[pick]=true;
        out_order[n++]=pick;
        /* Visit each bounded item once so every record receives the same rule. */
        for(j=0U;j<g->count;++j)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(g->edges[pick][j])indegree[j]-=1U;
    }
    *out_count=n;
    return UMI_STATUS_OK;
}
