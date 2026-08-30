/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/project_template_catalogue.c
 *
 * PURPOSE:
 *   Implement the project template catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Project Template Catalogue | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/project_template_catalogue.h"
#include "internal.h"
#include <string.h>
void umi_project_workspace_project_template_catalogue_init(UmiProjectWorkspaceProjectTemplateCatalogue *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_PROJECT_WORKSPACE_API_VERSION;
    value->revision = 1U;
}
bool umi_project_workspace_project_template_catalogue_contains(const UmiProjectWorkspaceProjectTemplateCatalogue *value, const char *id)
{
    size_t i;
    if (value == NULL || id == NULL) return false;
    for (i = 0U; i < value->count; ++i) if (strcmp(value->ids[i], id) == 0) return true;
    return false;
}
UmiStatus umi_project_workspace_project_template_catalogue_add(UmiProjectWorkspaceProjectTemplateCatalogue *value, const char *id)
{
    UmiStatus status;
    if (value == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_project_workspace_project_template_catalogue_contains(value, id)) return UMI_STATUS_ALREADY_EXISTS;
    if (value->count >= UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_pw_copy(value->ids[value->count], sizeof(value->ids[value->count]), id);
    if (status != UMI_STATUS_OK) return status;
    value->count += 1U;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_project_workspace_project_template_catalogue_at(const UmiProjectWorkspaceProjectTemplateCatalogue *value, size_t index, const char **out_id)
{
    if (value == NULL || out_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= value->count) return UMI_STATUS_NOT_FOUND;
    *out_id = value->ids[index]; return UMI_STATUS_OK;
}
