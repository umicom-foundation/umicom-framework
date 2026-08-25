/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_session.h
 * PURPOSE: Framework-owned Workspace Session contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_SESSION_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_SESSION_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
