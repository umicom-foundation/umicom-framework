/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_session.h
 *
 * PURPOSE:
 *   Publish the public workspace session contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_SESSION_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_SESSION_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace session data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceSession {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceSession;
    UmiStatus umi_project_workspace_workspace_session_init(UmiProjectWorkspaceWorkspaceSession *value,const char *id);
    UmiStatus umi_project_workspace_workspace_session_validate(const UmiProjectWorkspaceWorkspaceSession *value);
    UmiStatus umi_project_workspace_workspace_session_set_name(UmiProjectWorkspaceWorkspaceSession *value,const char *name);
    UmiStatus umi_project_workspace_workspace_session_set_detail(UmiProjectWorkspaceWorkspaceSession *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_session_set_state(UmiProjectWorkspaceWorkspaceSession *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_session_set_metric(UmiProjectWorkspaceWorkspaceSession *value,uint64_t metric);
    bool umi_project_workspace_workspace_session_same_identity(const UmiProjectWorkspaceWorkspaceSession *left,const UmiProjectWorkspaceWorkspaceSession *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_workspace_session_archive_encode(const UmiProjectWorkspaceWorkspaceSession *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_workspace_session_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceWorkspaceSession *value);

#ifdef __cplusplus
}
#endif
#endif
