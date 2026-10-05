/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_identity.h
 *
 * PURPOSE:
 *   Publish the public project identity contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_IDENTITY_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_IDENTITY_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace project identity data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceProjectIdentity {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectIdentity;
    UmiStatus umi_project_workspace_project_identity_init(UmiProjectWorkspaceProjectIdentity *value,const char *id);
    UmiStatus umi_project_workspace_project_identity_validate(const UmiProjectWorkspaceProjectIdentity *value);
    UmiStatus umi_project_workspace_project_identity_set_name(UmiProjectWorkspaceProjectIdentity *value,const char *name);
    UmiStatus umi_project_workspace_project_identity_set_detail(UmiProjectWorkspaceProjectIdentity *value,const char *detail);
    UmiStatus umi_project_workspace_project_identity_set_state(UmiProjectWorkspaceProjectIdentity *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_identity_set_metric(UmiProjectWorkspaceProjectIdentity *value,uint64_t metric);
    bool umi_project_workspace_project_identity_same_identity(const UmiProjectWorkspaceProjectIdentity *left,const UmiProjectWorkspaceProjectIdentity *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_project_identity_archive_encode(const UmiProjectWorkspaceProjectIdentity *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_project_identity_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceProjectIdentity *value);

#ifdef __cplusplus
}
#endif
#endif
