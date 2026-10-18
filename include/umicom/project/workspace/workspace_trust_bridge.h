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
    /** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
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

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiProjectWorkspaceWorkspaceTrustBridge object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_project_workspace_workspace_trust_bridge_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceWorkspaceTrustBridge *workspace, UmiProjectWorkspaceWorkspaceTrustBridge *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_project_workspace_workspace_trust_bridge_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiProjectWorkspaceWorkspaceTrustBridge *expected, UmiProjectWorkspaceWorkspaceTrustBridge *value);


#ifdef __cplusplus
}
#endif
#endif
