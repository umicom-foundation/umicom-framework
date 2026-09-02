/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_template_catalogue.h
 *
 * PURPOSE:
 *   Publish the public project template catalogue contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_TEMPLATE_CATALOGUE_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_TEMPLATE_CATALOGUE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace project template catalogue data shared with callers of
 * this public contract.
 */
    typedef struct UmiProjectWorkspaceProjectTemplateCatalogue {
        uint32_t structure_size;
        uint32_t api_version;
        char ids[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceProjectTemplateCatalogue;
    void umi_project_workspace_project_template_catalogue_init(UmiProjectWorkspaceProjectTemplateCatalogue *value);
    UmiStatus umi_project_workspace_project_template_catalogue_add(UmiProjectWorkspaceProjectTemplateCatalogue *value, const char *id);
    bool umi_project_workspace_project_template_catalogue_contains(const UmiProjectWorkspaceProjectTemplateCatalogue *value, const char *id);
    UmiStatus umi_project_workspace_project_template_catalogue_at(const UmiProjectWorkspaceProjectTemplateCatalogue *value, size_t index, const char **out_id);
#ifdef __cplusplus
}
#endif
#endif
