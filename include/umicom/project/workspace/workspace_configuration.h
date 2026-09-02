/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_configuration.h
 *
 * PURPOSE:
 *   Publish the public workspace configuration contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_CONFIGURATION_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_CONFIGURATION_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace configuration data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceConfiguration {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceConfiguration;
    UmiStatus umi_project_workspace_workspace_configuration_init(UmiProjectWorkspaceWorkspaceConfiguration *value,const char *id);
    UmiStatus umi_project_workspace_workspace_configuration_validate(const UmiProjectWorkspaceWorkspaceConfiguration *value);
    UmiStatus umi_project_workspace_workspace_configuration_set_name(UmiProjectWorkspaceWorkspaceConfiguration *value,const char *name);
    UmiStatus umi_project_workspace_workspace_configuration_set_detail(UmiProjectWorkspaceWorkspaceConfiguration *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_configuration_set_state(UmiProjectWorkspaceWorkspaceConfiguration *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_configuration_set_metric(UmiProjectWorkspaceWorkspaceConfiguration *value,uint64_t metric);
    bool umi_project_workspace_workspace_configuration_same_identity(const UmiProjectWorkspaceWorkspaceConfiguration *left,const UmiProjectWorkspaceWorkspaceConfiguration *right);
#ifdef __cplusplus
}
#endif
#endif
