/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_metadata.h
 *
 * PURPOSE:
 *   Publish the public project metadata contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_METADATA_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_METADATA_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace project metadata data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceProjectMetadata {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectMetadata;
    UmiStatus umi_project_workspace_project_metadata_init(UmiProjectWorkspaceProjectMetadata *value,const char *id);
    UmiStatus umi_project_workspace_project_metadata_validate(const UmiProjectWorkspaceProjectMetadata *value);
    UmiStatus umi_project_workspace_project_metadata_set_name(UmiProjectWorkspaceProjectMetadata *value,const char *name);
    UmiStatus umi_project_workspace_project_metadata_set_detail(UmiProjectWorkspaceProjectMetadata *value,const char *detail);
    UmiStatus umi_project_workspace_project_metadata_set_state(UmiProjectWorkspaceProjectMetadata *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_metadata_set_metric(UmiProjectWorkspaceProjectMetadata *value,uint64_t metric);
    bool umi_project_workspace_project_metadata_same_identity(const UmiProjectWorkspaceProjectMetadata *left,const UmiProjectWorkspaceProjectMetadata *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_project_metadata_archive_encode(const UmiProjectWorkspaceProjectMetadata *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_project_metadata_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceProjectMetadata *value);

#ifdef __cplusplus
}
#endif
#endif
