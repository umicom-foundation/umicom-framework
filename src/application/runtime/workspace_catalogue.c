/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/workspace_catalogue.c
 *
 * PURPOSE:
 *   Enumerate all canonical layouts without placing layout definitions in product repositories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/workspace_catalogue.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"

size_t umi_application_workspace_catalogue_count(void)
{
    size_t app_index;
    size_t total = 0U;
    for (app_index = 0U; app_index < umi_application_experience_catalogue_count(); ++app_index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(app_index);
        if (experience != NULL) total += experience->layout_count;
    }
    return total;
}

UmiStatus umi_application_workspace_catalogue_at(
    size_t index,
    UmiApplicationWorkspaceEntry *out_entry)
{
    size_t app_index;
    size_t cursor = 0U;
    if (out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (app_index = 0U; app_index < umi_application_experience_catalogue_count(); ++app_index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(app_index);
        size_t layout_index;
        if (experience == NULL) continue;
        for (layout_index = 0U; layout_index < experience->layout_count; ++layout_index) {
            if (cursor == index) {
                out_entry->application_id = experience->application_id;
                out_entry->application_name = experience->display_name;
                out_entry->layout = &experience->layouts[layout_index];
                return UMI_STATUS_OK;
            }
            cursor += 1U;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_application_workspace_catalogue_find(
    const char *application_id,
    const char *layout_id,
    UmiApplicationWorkspaceEntry *out_entry)
{
    const UmiApplicationExperienceDefinition *experience;
    const UmiExperienceLayoutDefinition *layout;
    if (application_id == NULL || layout_id == NULL || out_entry == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    experience = umi_application_experience_catalogue_find(application_id);
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;
    layout = umi_application_experience_layout_find(experience, layout_id);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    out_entry->application_id = experience->application_id;
    out_entry->application_name = experience->display_name;
    out_entry->layout = layout;
    return UMI_STATUS_OK;
}
