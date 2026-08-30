/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_identity.h
 *
 * PURPOSE:
 *   Publish the public project identity contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_IDENTITY_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_IDENTITY_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProjectIdentity {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectIdentity;
    UmiStatus umi_project_workspace_project_identity_init(UmiProjectWorkspaceProjectIdentity *value,const char *id);
    UmiStatus umi_project_workspace_project_identity_validate(const UmiProjectWorkspaceProjectIdentity *value);
    UmiStatus umi_project_workspace_project_identity_set_name(UmiProjectWorkspaceProjectIdentity *value,const char *name);
    UmiStatus umi_project_workspace_project_identity_set_detail(UmiProjectWorkspaceProjectIdentity *value,const char *detail);
    UmiStatus umi_project_workspace_project_identity_set_state(UmiProjectWorkspaceProjectIdentity *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_identity_set_metric(UmiProjectWorkspaceProjectIdentity *value,uint64_t metric);
    bool umi_project_workspace_project_identity_same_identity(const UmiProjectWorkspaceProjectIdentity *left,const UmiProjectWorkspaceProjectIdentity *right);
#ifdef __cplusplus
}
#endif
#endif
