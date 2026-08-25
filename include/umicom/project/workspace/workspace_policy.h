/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_policy.h
 * PURPOSE: Framework-owned Workspace Policy contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_POLICY_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_POLICY_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspacePolicy {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspacePolicy;
    UmiStatus umi_project_workspace_workspace_policy_init(UmiProjectWorkspaceWorkspacePolicy *value,const char *id);
    UmiStatus umi_project_workspace_workspace_policy_validate(const UmiProjectWorkspaceWorkspacePolicy *value);
    UmiStatus umi_project_workspace_workspace_policy_set_name(UmiProjectWorkspaceWorkspacePolicy *value,const char *name);
    UmiStatus umi_project_workspace_workspace_policy_set_detail(UmiProjectWorkspaceWorkspacePolicy *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_policy_set_state(UmiProjectWorkspaceWorkspacePolicy *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_policy_set_metric(UmiProjectWorkspaceWorkspacePolicy *value,uint64_t metric);
    bool umi_project_workspace_workspace_policy_same_identity(const UmiProjectWorkspaceWorkspacePolicy *left,const UmiProjectWorkspaceWorkspacePolicy *right);
#ifdef __cplusplus
}
#endif
#endif
