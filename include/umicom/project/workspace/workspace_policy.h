/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_policy.h
 *
 * PURPOSE:
 *   Publish the public workspace policy contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_POLICY_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_POLICY_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace policy data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceWorkspacePolicy {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspacePolicy;
    UmiStatus umi_project_workspace_workspace_policy_init(UmiProjectWorkspaceWorkspacePolicy *value,const char *id);
    UmiStatus umi_project_workspace_workspace_policy_validate(const UmiProjectWorkspaceWorkspacePolicy *value);
    UmiStatus umi_project_workspace_workspace_policy_set_name(UmiProjectWorkspaceWorkspacePolicy *value,const char *name);
    UmiStatus umi_project_workspace_workspace_policy_set_detail(UmiProjectWorkspaceWorkspacePolicy *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_policy_set_state(UmiProjectWorkspaceWorkspacePolicy *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_policy_set_metric(UmiProjectWorkspaceWorkspacePolicy *value,uint64_t metric);
    bool umi_project_workspace_workspace_policy_same_identity(const UmiProjectWorkspaceWorkspacePolicy *left,const UmiProjectWorkspaceWorkspacePolicy *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_workspace_policy_archive_encode(const UmiProjectWorkspaceWorkspacePolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_workspace_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceWorkspacePolicy *value);

#ifdef __cplusplus
}
#endif
#endif
