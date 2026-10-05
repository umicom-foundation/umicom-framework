/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_capability.h
 *
 * PURPOSE:
 *   Publish the public project capability contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_CAPABILITY_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_CAPABILITY_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace project capability data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceProjectCapability {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectCapability;
    UmiStatus umi_project_workspace_project_capability_init(UmiProjectWorkspaceProjectCapability *value,const char *id);
    UmiStatus umi_project_workspace_project_capability_validate(const UmiProjectWorkspaceProjectCapability *value);
    UmiStatus umi_project_workspace_project_capability_set_name(UmiProjectWorkspaceProjectCapability *value,const char *name);
    UmiStatus umi_project_workspace_project_capability_set_detail(UmiProjectWorkspaceProjectCapability *value,const char *detail);
    UmiStatus umi_project_workspace_project_capability_set_state(UmiProjectWorkspaceProjectCapability *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_capability_set_metric(UmiProjectWorkspaceProjectCapability *value,uint64_t metric);
    bool umi_project_workspace_project_capability_same_identity(const UmiProjectWorkspaceProjectCapability *left,const UmiProjectWorkspaceProjectCapability *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_project_capability_archive_encode(const UmiProjectWorkspaceProjectCapability *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_project_capability_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceProjectCapability *value);

#ifdef __cplusplus
}
#endif
#endif
