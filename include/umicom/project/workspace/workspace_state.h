/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_state.h
 *
 * PURPOSE:
 *   Publish the public workspace state contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_STATE_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_STATE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceState {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceState;
    UmiStatus umi_project_workspace_workspace_state_init(UmiProjectWorkspaceWorkspaceState *value,const char *id);
    UmiStatus umi_project_workspace_workspace_state_validate(const UmiProjectWorkspaceWorkspaceState *value);
    UmiStatus umi_project_workspace_workspace_state_set_name(UmiProjectWorkspaceWorkspaceState *value,const char *name);
    UmiStatus umi_project_workspace_workspace_state_set_detail(UmiProjectWorkspaceWorkspaceState *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_state_set_state(UmiProjectWorkspaceWorkspaceState *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_state_set_metric(UmiProjectWorkspaceWorkspaceState *value,uint64_t metric);
    bool umi_project_workspace_workspace_state_same_identity(const UmiProjectWorkspaceWorkspaceState *left,const UmiProjectWorkspaceWorkspaceState *right);
#ifdef __cplusplus
}
#endif
#endif
