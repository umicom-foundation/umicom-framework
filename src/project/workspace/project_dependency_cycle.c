/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/project_dependency_cycle.c
 *
 * PURPOSE:
 *   Implement the project dependency cycle behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_dependency_cycle.h"
/* Provide the visit operation used by this module and its client applications. */
static bool visit(const UmiProjectWorkspaceProjectGraph*g,size_t n,bool*seen,bool*stack) {
    size_t j;
    /* Apply this branch only when its contract condition is satisfied. */
    if(stack[n])return true;
    /* Apply this branch only when its contract condition is satisfied. */
    if(seen[n])return false;
    seen[n]=true;
    stack[n]=true;
    /* Visit each bounded item once so every record receives the same rule. */
    for(j=0U;j<g->count;++j)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(g->edges[n][j]&&visit(g,j,seen,stack))return true;
    stack[n]=false;
    return false;
}
/*
 * Provide the project workspace project dependency cycle detect operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_project_dependency_cycle_detect(const UmiProjectWorkspaceProjectGraph*g) {
    bool seen[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY]= {
        false
    };
    bool stack[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY]= {
        false
    };
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(g==NULL)return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<g->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(visit(g,i,seen,stack))return true;
    return false;
}
