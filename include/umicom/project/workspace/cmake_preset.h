/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_preset.h
 *
 * PURPOSE:
 *   Publish the public cmake preset contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_PRESET_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_PRESET_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace cmake preset data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceCmakePreset {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakePreset;
    UmiStatus umi_project_workspace_cmake_preset_init(UmiProjectWorkspaceCmakePreset *value,const char *id);
    UmiStatus umi_project_workspace_cmake_preset_validate(const UmiProjectWorkspaceCmakePreset *value);
    UmiStatus umi_project_workspace_cmake_preset_set_name(UmiProjectWorkspaceCmakePreset *value,const char *name);
    UmiStatus umi_project_workspace_cmake_preset_set_detail(UmiProjectWorkspaceCmakePreset *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_preset_set_state(UmiProjectWorkspaceCmakePreset *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_preset_set_metric(UmiProjectWorkspaceCmakePreset *value,uint64_t metric);
    bool umi_project_workspace_cmake_preset_same_identity(const UmiProjectWorkspaceCmakePreset *left,const UmiProjectWorkspaceCmakePreset *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_cmake_preset_archive_encode(const UmiProjectWorkspaceCmakePreset *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_cmake_preset_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceCmakePreset *value);

#ifdef __cplusplus
}
#endif
#endif
