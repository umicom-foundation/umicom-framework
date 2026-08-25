/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_profile.h
 * PURPOSE: Framework-owned Workspace Profile contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_PROFILE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceProfile;
    UmiStatus umi_project_workspace_workspace_profile_init(UmiProjectWorkspaceWorkspaceProfile *value,const char *id);
    UmiStatus umi_project_workspace_workspace_profile_validate(const UmiProjectWorkspaceWorkspaceProfile *value);
    UmiStatus umi_project_workspace_workspace_profile_set_name(UmiProjectWorkspaceWorkspaceProfile *value,const char *name);
    UmiStatus umi_project_workspace_workspace_profile_set_detail(UmiProjectWorkspaceWorkspaceProfile *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_profile_set_state(UmiProjectWorkspaceWorkspaceProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_profile_set_metric(UmiProjectWorkspaceWorkspaceProfile *value,uint64_t metric);
    bool umi_project_workspace_workspace_profile_same_identity(const UmiProjectWorkspaceWorkspaceProfile *left,const UmiProjectWorkspaceWorkspaceProfile *right);
#ifdef __cplusplus
}
#endif
#endif
