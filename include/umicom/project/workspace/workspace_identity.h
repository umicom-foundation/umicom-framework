/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_identity.h
 *
 * PURPOSE:
 *   Publish the public workspace identity contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_IDENTITY_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_IDENTITY_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace identity data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceIdentity {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceIdentity;
    UmiStatus umi_project_workspace_workspace_identity_init(UmiProjectWorkspaceWorkspaceIdentity *value,const char *id);
    UmiStatus umi_project_workspace_workspace_identity_validate(const UmiProjectWorkspaceWorkspaceIdentity *value);
    UmiStatus umi_project_workspace_workspace_identity_set_name(UmiProjectWorkspaceWorkspaceIdentity *value,const char *name);
    UmiStatus umi_project_workspace_workspace_identity_set_detail(UmiProjectWorkspaceWorkspaceIdentity *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_identity_set_state(UmiProjectWorkspaceWorkspaceIdentity *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_identity_set_metric(UmiProjectWorkspaceWorkspaceIdentity *value,uint64_t metric);
    bool umi_project_workspace_workspace_identity_same_identity(const UmiProjectWorkspaceWorkspaceIdentity *left,const UmiProjectWorkspaceWorkspaceIdentity *right);
#ifdef __cplusplus
}
#endif
#endif
