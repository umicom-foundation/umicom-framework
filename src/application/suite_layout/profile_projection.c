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

/* Convert one canonical layout into the smaller profile model used by the
 * workbench selector, optionally translating panel IDs during composition. */
UmiStatus umi_application_suite_layout_profile_project_resolved(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperienceLayoutDefinition *layout,
    int32_t order,
    UmiApplicationSuiteLayoutPaneResolver resolver,
    void *user_data,
    UmiUiWorkspaceProfileSnapshot *out_profile)
{
    const UmiExperienceLayoutDefinition *registered_layout;
    size_t index;
    int written;
    UmiStatus status;

    /* Projection requires canonical source metadata and caller-owned output. */
    if (experience == NULL || layout == NULL || out_profile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_experience_validate(experience);
    /* Invalid experience relationships cannot produce a trustworthy profile. */
    if (status != UMI_STATUS_OK) return status;
    registered_layout = umi_application_experience_layout_find(
        experience, layout->layout_id);
    /* Require the exact borrowed catalogue record so a same-named foreign or
     * short-lived layout cannot enter the workbench profile store. */
    if (registered_layout != layout) return UMI_STATUS_NOT_FOUND;

    (void)memset(out_profile, 0, sizeof(*out_profile));
    written = snprintf(out_profile->profile_id, sizeof(out_profile->profile_id),
                       "%s.%s", experience->application_id, layout->layout_id);
    /* The qualified profile ID is a persisted key and may not be truncated. */
    if (written < 0 || (size_t)written >= sizeof(out_profile->profile_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(out_profile->label, sizeof(out_profile->label), "%s",
                       layout->title);
    /* A partially copied display label would misdescribe the chosen layout. */
    if (written < 0 || (size_t)written >= sizeof(out_profile->label))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(out_profile->description, sizeof(out_profile->description),
                       "%s", layout->description);
    /* Keep the complete guidance text or reject the profile. */
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
    /* Resolve each layout panel through its owning experience before deciding
     * which workbench region becomes visible. */
    for (index = 0U; index < layout->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            umi_application_experience_panel_find(experience,
                                                  layout->panel_ids[index]);
        UmiUiPlacement placement;
        UmiUiWorkspacePanePlacement *pane;
        const char *resolved_pane_id;
        /* A layout recipe may never refer to a panel outside its experience. */
        if (panel == NULL) return UMI_STATUS_NOT_FOUND;
        status = umi_ui_placement_parse(panel->default_region, &placement);
        /* Preserve placement parsing errors instead of guessing a profile. */
        if (status != UMI_STATUS_OK) return status;
        /* Side and bottom placements control the profile's visible regions. */
        if (placement == UMI_UI_PLACEMENT_LEFT) out_profile->sidebar_visible = 1;
        else /* Apply this branch only when its contract condition is satisfied. */ if (placement == UMI_UI_PLACEMENT_RIGHT)
            out_profile->auxiliary_sidebar_visible = 1;
        else /* Apply this branch only when its contract condition is satisfied. */ if (placement == UMI_UI_PLACEMENT_BOTTOM)
            out_profile->bottom_panel_visible = 1;
        /* Centre editors and detached windows are rendered by their hosts,
         * not recorded as auxiliary workbench panes. */
        if (placement == UMI_UI_PLACEMENT_CENTRE ||
            placement == UMI_UI_PLACEMENT_FLOATING)
            continue;
        /* Bounded profile storage prevents a layout from overflowing panes. */
        if (out_profile->pane_count >= UMI_UI_WORKSPACE_PROFILE_MAX_PANES)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        resolved_pane_id = resolver != NULL ? resolver(panel, user_data) : NULL;
        /* A missing optional mapping keeps the canonical panel identity. */
        if (resolved_pane_id == NULL || resolved_pane_id[0] == '\0')
            resolved_pane_id = panel->panel_id;
        pane = &out_profile->panes[out_profile->pane_count++];
        written = snprintf(pane->pane_id, sizeof(pane->pane_id), "%s",
                           resolved_pane_id);
        /* Pane IDs participate in later lookups and must fit completely. */
        if (written < 0 || (size_t)written >= sizeof(pane->pane_id))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        pane->placement = placement;
        pane->order = (int32_t)index;
        pane->visible = 1;
    }
    return UMI_STATUS_OK;
}

/* Preserve the simple API by calling the resolver-aware implementation with
 * no mapping callback. */
UmiStatus umi_application_suite_layout_profile_project(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperienceLayoutDefinition *layout,
    int32_t order,
    UmiUiWorkspaceProfileSnapshot *out_profile)
{
    return umi_application_suite_layout_profile_project_resolved(
        experience, layout, order, NULL, NULL, out_profile);
}
