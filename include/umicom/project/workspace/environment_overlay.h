/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/environment_overlay.h
 * PURPOSE: Framework-owned Environment Overlay contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_ENVIRONMENT_OVERLAY_H
#define UMICOM_PROJECT_WORKSPACE_ENVIRONMENT_OVERLAY_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceEnvironmentOverlay {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceEnvironmentOverlay;
    UmiStatus umi_project_workspace_environment_overlay_init(UmiProjectWorkspaceEnvironmentOverlay *value,const char *id);
    UmiStatus umi_project_workspace_environment_overlay_validate(const UmiProjectWorkspaceEnvironmentOverlay *value);
    UmiStatus umi_project_workspace_environment_overlay_set_name(UmiProjectWorkspaceEnvironmentOverlay *value,const char *name);
    UmiStatus umi_project_workspace_environment_overlay_set_detail(UmiProjectWorkspaceEnvironmentOverlay *value,const char *detail);
    UmiStatus umi_project_workspace_environment_overlay_set_state(UmiProjectWorkspaceEnvironmentOverlay *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_environment_overlay_set_metric(UmiProjectWorkspaceEnvironmentOverlay *value,uint64_t metric);
    bool umi_project_workspace_environment_overlay_same_identity(const UmiProjectWorkspaceEnvironmentOverlay *left,const UmiProjectWorkspaceEnvironmentOverlay *right);
#ifdef __cplusplus
}
#endif
#endif
