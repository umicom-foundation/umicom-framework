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
#ifdef __cplusplus
}
#endif
#endif
