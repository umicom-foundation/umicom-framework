/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_state.h
 *
 * PURPOSE:
 *   Publish the public workspace state contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_STATE_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_STATE_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace state data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceState {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceState;
    UmiStatus umi_project_workspace_workspace_state_init(UmiProjectWorkspaceWorkspaceState *value,const char *id);
    UmiStatus umi_project_workspace_workspace_state_validate(const UmiProjectWorkspaceWorkspaceState *value);
    UmiStatus umi_project_workspace_workspace_state_set_name(UmiProjectWorkspaceWorkspaceState *value,const char *name);
    UmiStatus umi_project_workspace_workspace_state_set_detail(UmiProjectWorkspaceWorkspaceState *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_state_set_state(UmiProjectWorkspaceWorkspaceState *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_state_set_metric(UmiProjectWorkspaceWorkspaceState *value,uint64_t metric);
    bool umi_project_workspace_workspace_state_same_identity(const UmiProjectWorkspaceWorkspaceState *left,const UmiProjectWorkspaceWorkspaceState *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_workspace_state_archive_encode(const UmiProjectWorkspaceWorkspaceState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_workspace_state_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceWorkspaceState *value);

#ifdef __cplusplus
}
#endif
#endif
