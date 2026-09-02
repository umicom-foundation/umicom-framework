/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_root.h
 *
 * PURPOSE:
 *   Publish the public workspace root contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_ROOT_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_ROOT_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace root data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceRoot {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceRoot;
    UmiStatus umi_project_workspace_workspace_root_init(UmiProjectWorkspaceWorkspaceRoot *value,const char *id);
    UmiStatus umi_project_workspace_workspace_root_validate(const UmiProjectWorkspaceWorkspaceRoot *value);
    UmiStatus umi_project_workspace_workspace_root_set_name(UmiProjectWorkspaceWorkspaceRoot *value,const char *name);
    UmiStatus umi_project_workspace_workspace_root_set_detail(UmiProjectWorkspaceWorkspaceRoot *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_root_set_state(UmiProjectWorkspaceWorkspaceRoot *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_root_set_metric(UmiProjectWorkspaceWorkspaceRoot *value,uint64_t metric);
    bool umi_project_workspace_workspace_root_same_identity(const UmiProjectWorkspaceWorkspaceRoot *left,const UmiProjectWorkspaceWorkspaceRoot *right);
#ifdef __cplusplus
}
#endif
#endif
