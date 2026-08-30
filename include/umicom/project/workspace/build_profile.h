/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/build_profile.h
 *
 * PURPOSE:
 *   Publish the public build profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_BUILD_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_BUILD_PROFILE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceBuildProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceBuildProfile;
    UmiStatus umi_project_workspace_build_profile_init(UmiProjectWorkspaceBuildProfile *value,const char *id);
    UmiStatus umi_project_workspace_build_profile_validate(const UmiProjectWorkspaceBuildProfile *value);
    UmiStatus umi_project_workspace_build_profile_set_name(UmiProjectWorkspaceBuildProfile *value,const char *name);
    UmiStatus umi_project_workspace_build_profile_set_detail(UmiProjectWorkspaceBuildProfile *value,const char *detail);
    UmiStatus umi_project_workspace_build_profile_set_state(UmiProjectWorkspaceBuildProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_build_profile_set_metric(UmiProjectWorkspaceBuildProfile *value,uint64_t metric);
    bool umi_project_workspace_build_profile_same_identity(const UmiProjectWorkspaceBuildProfile *left,const UmiProjectWorkspaceBuildProfile *right);
#ifdef __cplusplus
}
#endif
#endif
