/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/debug_profile.h
 * PURPOSE: Framework-owned Debug Profile contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_DEBUG_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_DEBUG_PROFILE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceDebugProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceDebugProfile;
    UmiStatus umi_project_workspace_debug_profile_init(UmiProjectWorkspaceDebugProfile *value,const char *id);
    UmiStatus umi_project_workspace_debug_profile_validate(const UmiProjectWorkspaceDebugProfile *value);
    UmiStatus umi_project_workspace_debug_profile_set_name(UmiProjectWorkspaceDebugProfile *value,const char *name);
    UmiStatus umi_project_workspace_debug_profile_set_detail(UmiProjectWorkspaceDebugProfile *value,const char *detail);
    UmiStatus umi_project_workspace_debug_profile_set_state(UmiProjectWorkspaceDebugProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_debug_profile_set_metric(UmiProjectWorkspaceDebugProfile *value,uint64_t metric);
    bool umi_project_workspace_debug_profile_same_identity(const UmiProjectWorkspaceDebugProfile *left,const UmiProjectWorkspaceDebugProfile *right);
#ifdef __cplusplus
}
#endif
#endif
