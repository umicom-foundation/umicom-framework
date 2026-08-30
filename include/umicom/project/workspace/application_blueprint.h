/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/application_blueprint.h
 *
 * PURPOSE:
 *   Publish the public application blueprint contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_APPLICATION_BLUEPRINT_H
#define UMICOM_PROJECT_WORKSPACE_APPLICATION_BLUEPRINT_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceApplicationBlueprint {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceApplicationBlueprint;
    UmiStatus umi_project_workspace_application_blueprint_init(UmiProjectWorkspaceApplicationBlueprint *value,const char *id);
    UmiStatus umi_project_workspace_application_blueprint_validate(const UmiProjectWorkspaceApplicationBlueprint *value);
    UmiStatus umi_project_workspace_application_blueprint_set_name(UmiProjectWorkspaceApplicationBlueprint *value,const char *name);
    UmiStatus umi_project_workspace_application_blueprint_set_detail(UmiProjectWorkspaceApplicationBlueprint *value,const char *detail);
    UmiStatus umi_project_workspace_application_blueprint_set_state(UmiProjectWorkspaceApplicationBlueprint *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_application_blueprint_set_metric(UmiProjectWorkspaceApplicationBlueprint *value,uint64_t metric);
    bool umi_project_workspace_application_blueprint_same_identity(const UmiProjectWorkspaceApplicationBlueprint *left,const UmiProjectWorkspaceApplicationBlueprint *right);
#ifdef __cplusplus
}
#endif
#endif
