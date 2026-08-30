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
static size_t find_node(const UmiProjectWorkspaceProjectGraph*v,const char*id) {
    size_t i;
    for(i=0U;i<v->count;++i)if(strcmp(v->nodes[i],id)==0)return i;
    return SIZE_MAX;
}
void umi_project_workspace_project_graph_init(UmiProjectWorkspaceProjectGraph*v) {
    if(v!=NULL)(void)memset(v,0,sizeof(*v));
}
UmiStatus umi_project_workspace_project_graph_add_node(UmiProjectWorkspaceProjectGraph*v,const char*id) {
    UmiStatus s;
    if(v==NULL||id==NULL||id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(find_node(v,id)!=SIZE_MAX)return UMI_STATUS_ALREADY_EXISTS;
    if(v->count>=UMI_PROJECT_WORKSPACE_SMALL_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    s=umi_pw_copy(v->nodes[v->count],sizeof(v->nodes[v->count]),id);
    if(s==UMI_STATUS_OK)v->count+=1U;
    return s;
}
UmiStatus umi_project_workspace_project_graph_add_edge(UmiProjectWorkspaceProjectGraph*v,const char*from,const char*to) {
    size_t a,b;
    if(v==NULL||from==NULL||to==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    a=find_node(v,from);
    b=find_node(v,to);
    if(a==SIZE_MAX||b==SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    if(a==b)return UMI_STATUS_INVALID_ARGUMENT;
    v->edges[a][b]=true;
    return UMI_STATUS_OK;
}
bool umi_project_workspace_project_graph_has_edge(const UmiProjectWorkspaceProjectGraph*v,const char*from,const char*to) {
    size_t a,b;
    if(v==NULL||from==NULL||to==NULL)return false;
    a=find_node(v,from);
    b=find_node(v,to);
    return a!=SIZE_MAX&&b!=SIZE_MAX&&v->edges[a][b];
}
