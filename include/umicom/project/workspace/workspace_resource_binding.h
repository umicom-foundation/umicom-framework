/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_resource_binding.h
 *
 * PURPOSE:
 *   Publish the public workspace resource binding contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_RESOURCE_BINDING_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_RESOURCE_BINDING_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace resource binding data shared with callers of
 * this public contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceResourceBinding {
        char owner_id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char target_id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        uint64_t revision;
        bool enabled;
    }
    UmiProjectWorkspaceWorkspaceResourceBinding;
    UmiStatus umi_project_workspace_workspace_resource_binding_init(UmiProjectWorkspaceWorkspaceResourceBinding *value,const char *owner_id,const char *target_id);
    UmiStatus umi_project_workspace_workspace_resource_binding_retarget(UmiProjectWorkspaceWorkspaceResourceBinding *value,const char *target_id);
    bool umi_project_workspace_workspace_resource_binding_matches(const UmiProjectWorkspaceWorkspaceResourceBinding *value,const char *owner_id,const char *target_id);
#ifdef __cplusplus
}
#endif
#endif
