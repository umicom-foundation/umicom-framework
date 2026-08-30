/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_health.h
 *
 * PURPOSE:
 *   Publish the public workspace health contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_HEALTH_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_HEALTH_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceHealth {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceHealth;
    UmiStatus umi_project_workspace_workspace_health_init(UmiProjectWorkspaceWorkspaceHealth *value,const char *id);
    UmiStatus umi_project_workspace_workspace_health_validate(const UmiProjectWorkspaceWorkspaceHealth *value);
    UmiStatus umi_project_workspace_workspace_health_set_name(UmiProjectWorkspaceWorkspaceHealth *value,const char *name);
    UmiStatus umi_project_workspace_workspace_health_set_detail(UmiProjectWorkspaceWorkspaceHealth *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_health_set_state(UmiProjectWorkspaceWorkspaceHealth *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_health_set_metric(UmiProjectWorkspaceWorkspaceHealth *value,uint64_t metric);
    bool umi_project_workspace_workspace_health_same_identity(const UmiProjectWorkspaceWorkspaceHealth *left,const UmiProjectWorkspaceWorkspaceHealth *right);
#ifdef __cplusplus
}
#endif
#endif
