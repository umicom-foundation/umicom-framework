/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/environment_profile.h
 *
 * PURPOSE:
 *   Publish the public environment profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_ENVIRONMENT_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_ENVIRONMENT_PROFILE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceEnvironmentProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceEnvironmentProfile;
    UmiStatus umi_project_workspace_environment_profile_init(UmiProjectWorkspaceEnvironmentProfile *value,const char *id);
    UmiStatus umi_project_workspace_environment_profile_validate(const UmiProjectWorkspaceEnvironmentProfile *value);
    UmiStatus umi_project_workspace_environment_profile_set_name(UmiProjectWorkspaceEnvironmentProfile *value,const char *name);
    UmiStatus umi_project_workspace_environment_profile_set_detail(UmiProjectWorkspaceEnvironmentProfile *value,const char *detail);
    UmiStatus umi_project_workspace_environment_profile_set_state(UmiProjectWorkspaceEnvironmentProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_environment_profile_set_metric(UmiProjectWorkspaceEnvironmentProfile *value,uint64_t metric);
    bool umi_project_workspace_environment_profile_same_identity(const UmiProjectWorkspaceEnvironmentProfile *left,const UmiProjectWorkspaceEnvironmentProfile *right);
#ifdef __cplusplus
}
#endif
#endif
