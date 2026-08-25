#include "umicom/project/workspace/project_dependency_cycle.h"
static bool visit(const UmiProjectWorkspaceProjectGraph*g,size_t n,bool*seen,bool*stack) {
    size_t j;
    if(stack[n])return true;
    if(seen[n])return false;
    seen[n]=true;
    stack[n]=true;
    for(j=0U;j<g->count;++j)if(g->edges[n][j]&&visit(g,j,seen,stack))return true;
    stack[n]=false;
    return false;
}
bool umi_project_workspace_project_dependency_cycle_detect(const UmiProjectWorkspaceProjectGraph*g) {
    bool seen[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY]= {
        false
    };
    bool stack[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY]= {
        false
    };
    size_t i;
    if(g==NULL)return false;
    for(i=0U;i<g->count;++i)if(visit(g,i,seen,stack))return true;
    return false;
}
