/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_catalogue.h
 * PURPOSE: Framework-owned bounded Project Catalogue state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_CATALOGUE_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_CATALOGUE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProjectCatalogue {
        uint32_t structure_size;
        uint32_t api_version;
        char ids[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceProjectCatalogue;
    void umi_project_workspace_project_catalogue_init(UmiProjectWorkspaceProjectCatalogue *value);
    UmiStatus umi_project_workspace_project_catalogue_add(UmiProjectWorkspaceProjectCatalogue *value, const char *id);
    bool umi_project_workspace_project_catalogue_contains(const UmiProjectWorkspaceProjectCatalogue *value, const char *id);
    UmiStatus umi_project_workspace_project_catalogue_at(const UmiProjectWorkspaceProjectCatalogue *value, size_t index, const char **out_id);
#ifdef __cplusplus
}
#endif
#endif
