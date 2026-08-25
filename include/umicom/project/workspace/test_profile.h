/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/test_profile.h
 * PURPOSE: Framework-owned Test Profile contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_TEST_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_TEST_PROFILE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceTestProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceTestProfile;
    UmiStatus umi_project_workspace_test_profile_init(UmiProjectWorkspaceTestProfile *value,const char *id);
    UmiStatus umi_project_workspace_test_profile_validate(const UmiProjectWorkspaceTestProfile *value);
    UmiStatus umi_project_workspace_test_profile_set_name(UmiProjectWorkspaceTestProfile *value,const char *name);
    UmiStatus umi_project_workspace_test_profile_set_detail(UmiProjectWorkspaceTestProfile *value,const char *detail);
    UmiStatus umi_project_workspace_test_profile_set_state(UmiProjectWorkspaceTestProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_test_profile_set_metric(UmiProjectWorkspaceTestProfile *value,uint64_t metric);
    bool umi_project_workspace_test_profile_same_identity(const UmiProjectWorkspaceTestProfile *left,const UmiProjectWorkspaceTestProfile *right);
#ifdef __cplusplus
}
#endif
#endif
