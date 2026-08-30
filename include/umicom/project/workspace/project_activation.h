/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_activation.h
 *
 * PURPOSE:
 *   Publish the public project activation contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_ACTIVATION_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_ACTIVATION_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProjectActivation {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectActivation;
    UmiStatus umi_project_workspace_project_activation_init(UmiProjectWorkspaceProjectActivation *value,const char *id);
    UmiStatus umi_project_workspace_project_activation_validate(const UmiProjectWorkspaceProjectActivation *value);
    UmiStatus umi_project_workspace_project_activation_set_name(UmiProjectWorkspaceProjectActivation *value,const char *name);
    UmiStatus umi_project_workspace_project_activation_set_detail(UmiProjectWorkspaceProjectActivation *value,const char *detail);
    UmiStatus umi_project_workspace_project_activation_set_state(UmiProjectWorkspaceProjectActivation *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_activation_set_metric(UmiProjectWorkspaceProjectActivation *value,uint64_t metric);
    bool umi_project_workspace_project_activation_same_identity(const UmiProjectWorkspaceProjectActivation *left,const UmiProjectWorkspaceProjectActivation *right);
#ifdef __cplusplus
}
#endif
#endif
