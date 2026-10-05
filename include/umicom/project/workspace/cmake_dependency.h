/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_dependency.h
 *
 * PURPOSE:
 *   Publish the public cmake dependency contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_DEPENDENCY_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_DEPENDENCY_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace cmake dependency data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceCmakeDependency {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakeDependency;
    UmiStatus umi_project_workspace_cmake_dependency_init(UmiProjectWorkspaceCmakeDependency *value,const char *id);
    UmiStatus umi_project_workspace_cmake_dependency_validate(const UmiProjectWorkspaceCmakeDependency *value);
    UmiStatus umi_project_workspace_cmake_dependency_set_name(UmiProjectWorkspaceCmakeDependency *value,const char *name);
    UmiStatus umi_project_workspace_cmake_dependency_set_detail(UmiProjectWorkspaceCmakeDependency *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_dependency_set_state(UmiProjectWorkspaceCmakeDependency *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_dependency_set_metric(UmiProjectWorkspaceCmakeDependency *value,uint64_t metric);
    bool umi_project_workspace_cmake_dependency_same_identity(const UmiProjectWorkspaceCmakeDependency *left,const UmiProjectWorkspaceCmakeDependency *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_cmake_dependency_archive_encode(const UmiProjectWorkspaceCmakeDependency *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_cmake_dependency_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceCmakeDependency *value);

#ifdef __cplusplus
}
#endif
#endif
