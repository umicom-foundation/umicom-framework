/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_model.h
 *
 * PURPOSE:
 *   Publish the public cmake model contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_MODEL_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_MODEL_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace cmake model data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceCmakeModel {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakeModel;
    UmiStatus umi_project_workspace_cmake_model_init(UmiProjectWorkspaceCmakeModel *value,const char *id);
    UmiStatus umi_project_workspace_cmake_model_validate(const UmiProjectWorkspaceCmakeModel *value);
    UmiStatus umi_project_workspace_cmake_model_set_name(UmiProjectWorkspaceCmakeModel *value,const char *name);
    UmiStatus umi_project_workspace_cmake_model_set_detail(UmiProjectWorkspaceCmakeModel *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_model_set_state(UmiProjectWorkspaceCmakeModel *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_model_set_metric(UmiProjectWorkspaceCmakeModel *value,uint64_t metric);
    bool umi_project_workspace_cmake_model_same_identity(const UmiProjectWorkspaceCmakeModel *left,const UmiProjectWorkspaceCmakeModel *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_cmake_model_archive_encode(const UmiProjectWorkspaceCmakeModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_cmake_model_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceCmakeModel *value);

#ifdef __cplusplus
}
#endif
#endif
