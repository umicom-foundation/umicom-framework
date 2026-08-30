/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_profile_catalogue.h
 *
 * PURPOSE:
 *   Publish the public workspace profile catalogue contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_PROFILE_CATALOGUE_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_PROFILE_CATALOGUE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceProfileCatalogue {
        uint32_t structure_size;
        uint32_t api_version;
        char ids[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceWorkspaceProfileCatalogue;
    void umi_project_workspace_workspace_profile_catalogue_init(UmiProjectWorkspaceWorkspaceProfileCatalogue *value);
    UmiStatus umi_project_workspace_workspace_profile_catalogue_add(UmiProjectWorkspaceWorkspaceProfileCatalogue *value, const char *id);
    bool umi_project_workspace_workspace_profile_catalogue_contains(const UmiProjectWorkspaceWorkspaceProfileCatalogue *value, const char *id);
    UmiStatus umi_project_workspace_workspace_profile_catalogue_at(const UmiProjectWorkspaceWorkspaceProfileCatalogue *value, size_t index, const char **out_id);
#ifdef __cplusplus
}
#endif
#endif
