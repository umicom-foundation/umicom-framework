/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_root_catalogue.h
 * PURPOSE: Framework-owned bounded Workspace Root Catalogue state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_ROOT_CATALOGUE_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_ROOT_CATALOGUE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceRootCatalogue {
        uint32_t structure_size;
        uint32_t api_version;
        char ids[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceWorkspaceRootCatalogue;
    void umi_project_workspace_workspace_root_catalogue_init(UmiProjectWorkspaceWorkspaceRootCatalogue *value);
    UmiStatus umi_project_workspace_workspace_root_catalogue_add(UmiProjectWorkspaceWorkspaceRootCatalogue *value, const char *id);
    bool umi_project_workspace_workspace_root_catalogue_contains(const UmiProjectWorkspaceWorkspaceRootCatalogue *value, const char *id);
    UmiStatus umi_project_workspace_workspace_root_catalogue_at(const UmiProjectWorkspaceWorkspaceRootCatalogue *value, size_t index, const char **out_id);
#ifdef __cplusplus
}
#endif
#endif
