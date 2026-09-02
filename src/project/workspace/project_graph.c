/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/project_graph.c
 *
 * PURPOSE:
 *   Implement the project graph behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_graph.h"
#include "internal.h"
#include <string.h>
/* Provide the find node operation used by this module and its client applications. */
static size_t find_node(const UmiProjectWorkspaceProjectGraph*v,const char*id) {
    size_t i;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<v->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(v->nodes[i],id)==0)return i;
    return SIZE_MAX;
}
/*
 * Initialise project workspace project graph from caller-provided values so later
 * operations receive a known state.
 */
void umi_project_workspace_project_graph_init(UmiProjectWorkspaceProjectGraph*v) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(v!=NULL)(void)memset(v,0,sizeof(*v));
}
/*
 * Provide the project workspace project graph add node operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_project_graph_add_node(UmiProjectWorkspaceProjectGraph*v,const char*id) {
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(v==NULL||id==NULL||id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(find_node(v,id)!=SIZE_MAX)return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(v->count>=UMI_PROJECT_WORKSPACE_SMALL_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    s=umi_pw_copy(v->nodes[v->count],sizeof(v->nodes[v->count]),id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(s==UMI_STATUS_OK)v->count+=1U;
    return s;
}
/*
 * Provide the project workspace project graph add edge operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_project_graph_add_edge(UmiProjectWorkspaceProjectGraph*v,const char*from,const char*to) {
    size_t a,b;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(v==NULL||from==NULL||to==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    a=find_node(v,from);
    b=find_node(v,to);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(a==SIZE_MAX||b==SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(a==b)return UMI_STATUS_INVALID_ARGUMENT;
    v->edges[a][b]=true;
    return UMI_STATUS_OK;
}
/*
 * Provide the project workspace project graph has edge operation used by this module and
 * its client applications.
 */
bool umi_project_workspace_project_graph_has_edge(const UmiProjectWorkspaceProjectGraph*v,const char*from,const char*to) {
    size_t a,b;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(v==NULL||from==NULL||to==NULL)return false;
    a=find_node(v,from);
    b=find_node(v,to);
    return a!=SIZE_MAX&&b!=SIZE_MAX&&v->edges[a][b];
}
