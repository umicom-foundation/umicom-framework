/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/projection.c
 *
 * PURPOSE:
 *   Project application panels into tab-group geometry using the existing UI workspace layout contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/projection.h"

#include <stdio.h>
#include <string.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/geometry.h"

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    if (written < 0 || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_suite_layout_project(
    const UmiApplicationExperienceDefinition *experience,
    const char *layout_id,
    UmiUiWorkspaceLayout *out_layout)
{
    const UmiExperienceLayoutDefinition *definition;
    char qualified_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char reason[256U];
    size_t index;
    UmiStatus status;
    int written;
    if (experience == NULL || layout_id == NULL || out_layout == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_experience_validate(experience);
    if (status != UMI_STATUS_OK) return status;
    definition = umi_application_experience_layout_find(experience, layout_id);
    if (definition == NULL) return UMI_STATUS_NOT_FOUND;
    written = snprintf(qualified_id, sizeof(qualified_id), "%s.%s",
                       experience->application_id, definition->layout_id);
    if (written < 0 || (size_t)written >= sizeof(qualified_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_ui_workspace_layout_init(out_layout, qualified_id, definition->title);
    if (status != UMI_STATUS_OK) return status;
    /* Workspace layouts initialise locked. Projection is a controlled build
     * phase, so unlock before adding windows; callers may lock the completed
     * layout afterwards through the existing Framework API. */
    status = umi_ui_workspace_layout_set_locked(out_layout, false);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < definition->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            umi_application_experience_panel_find(experience,
                                                  definition->panel_ids[index]);
        UmiUiWorkspaceWindow window;
        UmiUiPlacement placement;
        UmiApplicationSuiteLayoutRect rect;
        if (panel == NULL) return UMI_STATUS_NOT_FOUND;
        status = umi_ui_placement_parse(panel->default_region, &placement);
        if (status != UMI_STATUS_OK ||
            !umi_application_suite_layout_region_supported(placement))
            return UMI_STATUS_INVALID_STATE;
        rect = umi_application_suite_layout_region_rect(placement);
        (void)memset(&window, 0, sizeof(window));
        status = copy_text(window.window_id, sizeof(window.window_id), panel->panel_id);
        if (status == UMI_STATUS_OK)
            status = copy_text(window.title, sizeof(window.title), panel->title);
        if (status == UMI_STATUS_OK)
            status = copy_text(window.tool_id, sizeof(window.tool_id), panel->panel_id);
        if (status == UMI_STATUS_OK)
            status = copy_text(window.group_id, sizeof(window.group_id), panel->default_region);
        if (status != UMI_STATUS_OK) return status;
        window.x = rect.x; window.y = rect.y;
        window.width = rect.width; window.height = rect.height;
        window.visible = true;
        window.floating = placement == UMI_UI_PLACEMENT_FLOATING;
        window.maximised = false;
        window.closable = (panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) == 0U;
        window.z_order = (int32_t)index;
        status = umi_ui_workspace_layout_add_window(out_layout, &window);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_ui_workspace_layout_validate(out_layout, reason, sizeof(reason));
}

UmiStatus umi_application_suite_layout_project_default(
    const char *application_id,
    UmiUiWorkspaceLayout *out_layout)
{
    const UmiApplicationExperienceDefinition *experience;
    if (application_id == NULL || out_layout == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    experience = umi_application_experience_catalogue_find(application_id);
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;
    return umi_application_suite_layout_project(
        experience, experience->default_layout_id, out_layout);
}
