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
/*
 * Initialise project workspace project template catalogue from caller-provided values so
 * later operations receive a known state.
 */
void umi_project_workspace_project_template_catalogue_init(UmiProjectWorkspaceProjectTemplateCatalogue *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_PROJECT_WORKSPACE_API_VERSION;
    value->revision = 1U;
}
/*
 * Provide the project workspace project template catalogue contains operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_project_template_catalogue_contains(const UmiProjectWorkspaceProjectTemplateCatalogue *value, const char *id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || id == NULL) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < value->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(value->ids[i], id) == 0) return true;
    return false;
}
/*
 * Add project workspace project template catalogue only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_project_workspace_project_template_catalogue_add(UmiProjectWorkspaceProjectTemplateCatalogue *value, const char *id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_project_workspace_project_template_catalogue_contains(value, id)) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value->count >= UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_pw_copy(value->ids[value->count], sizeof(value->ids[value->count]), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->count += 1U;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Find project workspace project template catalogue while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_project_workspace_project_template_catalogue_at(const UmiProjectWorkspaceProjectTemplateCatalogue *value, size_t index, const char **out_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= value->count) return UMI_STATUS_NOT_FOUND;
    *out_id = value->ids[index]; return UMI_STATUS_OK;
}
