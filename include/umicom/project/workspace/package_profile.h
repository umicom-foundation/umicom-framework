/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/package_profile.h
 *
 * PURPOSE:
 *   Publish the public package profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PACKAGE_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_PACKAGE_PROFILE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspacePackageProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspacePackageProfile;
    UmiStatus umi_project_workspace_package_profile_init(UmiProjectWorkspacePackageProfile *value,const char *id);
    UmiStatus umi_project_workspace_package_profile_validate(const UmiProjectWorkspacePackageProfile *value);
    UmiStatus umi_project_workspace_package_profile_set_name(UmiProjectWorkspacePackageProfile *value,const char *name);
    UmiStatus umi_project_workspace_package_profile_set_detail(UmiProjectWorkspacePackageProfile *value,const char *detail);
    UmiStatus umi_project_workspace_package_profile_set_state(UmiProjectWorkspacePackageProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_package_profile_set_metric(UmiProjectWorkspacePackageProfile *value,uint64_t metric);
    bool umi_project_workspace_package_profile_same_identity(const UmiProjectWorkspacePackageProfile *left,const UmiProjectWorkspacePackageProfile *right);
#ifdef __cplusplus
}
#endif
#endif
