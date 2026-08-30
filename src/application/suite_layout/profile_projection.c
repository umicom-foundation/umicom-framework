/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/profile_projection.c
 *
 * PURPOSE:
 *   Build reusable workbench profiles from canonical application experience
 *   metadata while allowing thin products to resolve their composition-only
 *   pane identifiers without duplicating layout behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/profile_projection.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_application_suite_layout_profile_project_resolved(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperienceLayoutDefinition *layout,
    int32_t order,
    UmiApplicationSuiteLayoutPaneResolver resolver,
    void *user_data,
    UmiUiWorkspaceProfileSnapshot *out_profile)
{
    size_t index;
    int written;
    if (experience == NULL || layout == NULL || out_profile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_profile, 0, sizeof(*out_profile));
    written = snprintf(out_profile->profile_id, sizeof(out_profile->profile_id),
                       "%s.%s", experience->application_id, layout->layout_id);
    if (written < 0 || (size_t)written >= sizeof(out_profile->profile_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(out_profile->label, sizeof(out_profile->label), "%s",
                       layout->title);
    if (written < 0 || (size_t)written >= sizeof(out_profile->label))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(out_profile->description, sizeof(out_profile->description),
                       "%s", layout->description);
    if (written < 0 || (size_t)written >= sizeof(out_profile->description))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(out_profile->icon_name, sizeof(out_profile->icon_name),
                   "%s", "view-grid-symbolic");
    out_profile->sidebar_size = 300;
    out_profile->auxiliary_sidebar_size = 380;
    out_profile->bottom_panel_size = 280;
    out_profile->order = order;
    out_profile->built_in = 1;
    out_profile->locked = 1;
    for (index = 0U; index < layout->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            umi_application_experience_panel_find(experience,
                                                  layout->panel_ids[index]);
        UmiUiPlacement placement;
        UmiUiWorkspacePanePlacement *pane;
        const char *resolved_pane_id;
        UmiStatus status;
        if (panel == NULL) return UMI_STATUS_NOT_FOUND;
        status = umi_ui_placement_parse(panel->default_region, &placement);
        if (status != UMI_STATUS_OK) return status;
        if (placement == UMI_UI_PLACEMENT_LEFT) out_profile->sidebar_visible = 1;
        else if (placement == UMI_UI_PLACEMENT_RIGHT)
            out_profile->auxiliary_sidebar_visible = 1;
        else if (placement == UMI_UI_PLACEMENT_BOTTOM)
            out_profile->bottom_panel_visible = 1;
        if (placement == UMI_UI_PLACEMENT_CENTRE ||
            placement == UMI_UI_PLACEMENT_FLOATING)
            continue;
        if (out_profile->pane_count >= UMI_UI_WORKSPACE_PROFILE_MAX_PANES)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        resolved_pane_id = resolver != NULL ? resolver(panel, user_data) : NULL;
        if (resolved_pane_id == NULL || resolved_pane_id[0] == '\0')
            resolved_pane_id = panel->panel_id;
        pane = &out_profile->panes[out_profile->pane_count++];
        written = snprintf(pane->pane_id, sizeof(pane->pane_id), "%s",
                           resolved_pane_id);
        if (written < 0 || (size_t)written >= sizeof(pane->pane_id))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        pane->placement = placement;
        pane->order = (int32_t)index;
        pane->visible = 1;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_suite_layout_profile_project(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperienceLayoutDefinition *layout,
    int32_t order,
    UmiUiWorkspaceProfileSnapshot *out_profile)
{
    return umi_application_suite_layout_profile_project_resolved(
        experience, layout, order, NULL, NULL, out_profile);
}
