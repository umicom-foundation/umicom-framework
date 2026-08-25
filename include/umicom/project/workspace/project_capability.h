/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_capability.h
 * PURPOSE: Framework-owned Project Capability contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_CAPABILITY_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_CAPABILITY_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProjectCapability {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectCapability;
    UmiStatus umi_project_workspace_project_capability_init(UmiProjectWorkspaceProjectCapability *value,const char *id);
    UmiStatus umi_project_workspace_project_capability_validate(const UmiProjectWorkspaceProjectCapability *value);
    UmiStatus umi_project_workspace_project_capability_set_name(UmiProjectWorkspaceProjectCapability *value,const char *name);
    UmiStatus umi_project_workspace_project_capability_set_detail(UmiProjectWorkspaceProjectCapability *value,const char *detail);
    UmiStatus umi_project_workspace_project_capability_set_state(UmiProjectWorkspaceProjectCapability *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_capability_set_metric(UmiProjectWorkspaceProjectCapability *value,uint64_t metric);
    bool umi_project_workspace_project_capability_same_identity(const UmiProjectWorkspaceProjectCapability *left,const UmiProjectWorkspaceProjectCapability *right);
#ifdef __cplusplus
}
#endif
#endif
