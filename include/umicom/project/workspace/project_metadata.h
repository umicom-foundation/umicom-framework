/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_metadata.h
 *
 * PURPOSE:
 *   Publish the public project metadata contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_METADATA_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_METADATA_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProjectMetadata {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectMetadata;
    UmiStatus umi_project_workspace_project_metadata_init(UmiProjectWorkspaceProjectMetadata *value,const char *id);
    UmiStatus umi_project_workspace_project_metadata_validate(const UmiProjectWorkspaceProjectMetadata *value);
    UmiStatus umi_project_workspace_project_metadata_set_name(UmiProjectWorkspaceProjectMetadata *value,const char *name);
    UmiStatus umi_project_workspace_project_metadata_set_detail(UmiProjectWorkspaceProjectMetadata *value,const char *detail);
    UmiStatus umi_project_workspace_project_metadata_set_state(UmiProjectWorkspaceProjectMetadata *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_metadata_set_metric(UmiProjectWorkspaceProjectMetadata *value,uint64_t metric);
    bool umi_project_workspace_project_metadata_same_identity(const UmiProjectWorkspaceProjectMetadata *left,const UmiProjectWorkspaceProjectMetadata *right);
#ifdef __cplusplus
}
#endif
#endif
