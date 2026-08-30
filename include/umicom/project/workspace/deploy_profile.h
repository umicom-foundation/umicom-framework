/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/deploy_profile.h
 *
 * PURPOSE:
 *   Publish the public deploy profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_DEPLOY_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_DEPLOY_PROFILE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceDeployProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceDeployProfile;
    UmiStatus umi_project_workspace_deploy_profile_init(UmiProjectWorkspaceDeployProfile *value,const char *id);
    UmiStatus umi_project_workspace_deploy_profile_validate(const UmiProjectWorkspaceDeployProfile *value);
    UmiStatus umi_project_workspace_deploy_profile_set_name(UmiProjectWorkspaceDeployProfile *value,const char *name);
    UmiStatus umi_project_workspace_deploy_profile_set_detail(UmiProjectWorkspaceDeployProfile *value,const char *detail);
    UmiStatus umi_project_workspace_deploy_profile_set_state(UmiProjectWorkspaceDeployProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_deploy_profile_set_metric(UmiProjectWorkspaceDeployProfile *value,uint64_t metric);
    bool umi_project_workspace_deploy_profile_same_identity(const UmiProjectWorkspaceDeployProfile *left,const UmiProjectWorkspaceDeployProfile *right);
#ifdef __cplusplus
}
#endif
#endif
