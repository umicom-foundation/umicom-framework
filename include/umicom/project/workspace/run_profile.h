/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/run_profile.h
 * PURPOSE: Framework-owned Run Profile contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_RUN_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_RUN_PROFILE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceRunProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceRunProfile;
    UmiStatus umi_project_workspace_run_profile_init(UmiProjectWorkspaceRunProfile *value,const char *id);
    UmiStatus umi_project_workspace_run_profile_validate(const UmiProjectWorkspaceRunProfile *value);
    UmiStatus umi_project_workspace_run_profile_set_name(UmiProjectWorkspaceRunProfile *value,const char *name);
    UmiStatus umi_project_workspace_run_profile_set_detail(UmiProjectWorkspaceRunProfile *value,const char *detail);
    UmiStatus umi_project_workspace_run_profile_set_state(UmiProjectWorkspaceRunProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_run_profile_set_metric(UmiProjectWorkspaceRunProfile *value,uint64_t metric);
    bool umi_project_workspace_run_profile_same_identity(const UmiProjectWorkspaceRunProfile *left,const UmiProjectWorkspaceRunProfile *right);
#ifdef __cplusplus
}
#endif
#endif
