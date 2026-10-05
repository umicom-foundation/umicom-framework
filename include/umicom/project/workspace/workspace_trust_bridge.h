/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_trust_bridge.h
 *
 * PURPOSE:
 *   Publish the public workspace trust bridge contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_TRUST_BRIDGE_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_TRUST_BRIDGE_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace trust bridge data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceTrustBridge {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceTrustBridge;
    UmiStatus umi_project_workspace_workspace_trust_bridge_init(UmiProjectWorkspaceWorkspaceTrustBridge *value,const char *id);
    UmiStatus umi_project_workspace_workspace_trust_bridge_validate(const UmiProjectWorkspaceWorkspaceTrustBridge *value);
    UmiStatus umi_project_workspace_workspace_trust_bridge_set_name(UmiProjectWorkspaceWorkspaceTrustBridge *value,const char *name);
    UmiStatus umi_project_workspace_workspace_trust_bridge_set_detail(UmiProjectWorkspaceWorkspaceTrustBridge *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_trust_bridge_set_state(UmiProjectWorkspaceWorkspaceTrustBridge *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_trust_bridge_set_metric(UmiProjectWorkspaceWorkspaceTrustBridge *value,uint64_t metric);
    bool umi_project_workspace_workspace_trust_bridge_same_identity(const UmiProjectWorkspaceWorkspaceTrustBridge *left,const UmiProjectWorkspaceWorkspaceTrustBridge *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_workspace_trust_bridge_archive_encode(const UmiProjectWorkspaceWorkspaceTrustBridge *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_workspace_trust_bridge_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceWorkspaceTrustBridge *value);

#ifdef __cplusplus
}
#endif
#endif
