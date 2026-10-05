/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_target.h
 *
 * PURPOSE:
 *   Publish the public cmake target contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_TARGET_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_TARGET_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace cmake target data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceCmakeTarget {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakeTarget;
    UmiStatus umi_project_workspace_cmake_target_init(UmiProjectWorkspaceCmakeTarget *value,const char *id);
    UmiStatus umi_project_workspace_cmake_target_validate(const UmiProjectWorkspaceCmakeTarget *value);
    UmiStatus umi_project_workspace_cmake_target_set_name(UmiProjectWorkspaceCmakeTarget *value,const char *name);
    UmiStatus umi_project_workspace_cmake_target_set_detail(UmiProjectWorkspaceCmakeTarget *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_target_set_state(UmiProjectWorkspaceCmakeTarget *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_target_set_metric(UmiProjectWorkspaceCmakeTarget *value,uint64_t metric);
    bool umi_project_workspace_cmake_target_same_identity(const UmiProjectWorkspaceCmakeTarget *left,const UmiProjectWorkspaceCmakeTarget *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_cmake_target_archive_encode(const UmiProjectWorkspaceCmakeTarget *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_cmake_target_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceCmakeTarget *value);

#ifdef __cplusplus
}
#endif
#endif
