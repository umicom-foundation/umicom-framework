/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_dependency_edge.h
 *
 * PURPOSE:
 *   Publish the public project dependency edge contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_DEPENDENCY_EDGE_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_DEPENDENCY_EDGE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace project dependency edge data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceProjectDependencyEdge {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectDependencyEdge;
    UmiStatus umi_project_workspace_project_dependency_edge_init(UmiProjectWorkspaceProjectDependencyEdge *value,const char *id);
    UmiStatus umi_project_workspace_project_dependency_edge_validate(const UmiProjectWorkspaceProjectDependencyEdge *value);
    UmiStatus umi_project_workspace_project_dependency_edge_set_name(UmiProjectWorkspaceProjectDependencyEdge *value,const char *name);
    UmiStatus umi_project_workspace_project_dependency_edge_set_detail(UmiProjectWorkspaceProjectDependencyEdge *value,const char *detail);
    UmiStatus umi_project_workspace_project_dependency_edge_set_state(UmiProjectWorkspaceProjectDependencyEdge *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_dependency_edge_set_metric(UmiProjectWorkspaceProjectDependencyEdge *value,uint64_t metric);
    bool umi_project_workspace_project_dependency_edge_same_identity(const UmiProjectWorkspaceProjectDependencyEdge *left,const UmiProjectWorkspaceProjectDependencyEdge *right);
#ifdef __cplusplus
}
#endif
#endif
