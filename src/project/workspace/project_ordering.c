#include "umicom/project/workspace/project_ordering.h"
UmiStatus umi_project_workspace_project_ordering_topological(const UmiProjectWorkspaceProjectGraph*g,size_t*out_order,size_t capacity,size_t*out_count) {
    size_t indegree[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY]= {
        0U
    };
    bool emitted[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY]= {
        false
    };
    size_t i,j,n=0U;
    if(g==NULL||out_count==NULL||(g->count>0U&&out_order==NULL))return UMI_STATUS_INVALID_ARGUMENT;
    if(capacity<g->count)return UMI_STATUS_CAPACITY_EXCEEDED;
    for(i=0U;i<g->count;++i)for(j=0U;j<g->count;++j)if(g->edges[i][j])indegree[j]+=1U;
    while(n<g->count) {
        size_t pick=SIZE_MAX;
        for(i=0U;i<g->count;++i)if(!emitted[i]&&indegree[i]==0U) {
            pick=i;
            break;
        }
        if(pick==SIZE_MAX)return UMI_STATUS_INVALID_STATE;
        emitted[pick]=true;
        out_order[n++]=pick;
        for(j=0U;j<g->count;++j)if(g->edges[pick][j])indegree[j]-=1U;
    }
    *out_count=n;
    return UMI_STATUS_OK;
}
