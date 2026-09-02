/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_graph.h
 *
 * PURPOSE:
 *   Publish the public project graph contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_GRAPH_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_GRAPH_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace project graph data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceProjectGraph {
        char nodes[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY][UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        bool edges[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY][UMI_PROJECT_WORKSPACE_SMALL_CAPACITY];
        size_t count;
    }
    UmiProjectWorkspaceProjectGraph;
    void umi_project_workspace_project_graph_init(UmiProjectWorkspaceProjectGraph*value);
    UmiStatus umi_project_workspace_project_graph_add_node(UmiProjectWorkspaceProjectGraph*value,const char*id);
    UmiStatus umi_project_workspace_project_graph_add_edge(UmiProjectWorkspaceProjectGraph*value,const char*from,const char*to);
    bool umi_project_workspace_project_graph_has_edge(const UmiProjectWorkspaceProjectGraph*value,const char*from,const char*to);
#ifdef __cplusplus
}
#endif
#endif
