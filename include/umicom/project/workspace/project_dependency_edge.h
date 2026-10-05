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
#include "umicom/base/value_archive.h"
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
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_project_dependency_edge_archive_encode(const UmiProjectWorkspaceProjectDependencyEdge *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_project_dependency_edge_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceProjectDependencyEdge *value);

#ifdef __cplusplus
}
#endif
#endif
