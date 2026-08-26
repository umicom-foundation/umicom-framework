/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/layout_session.c
 *
 * PURPOSE:
 *   Implement deterministic layout selection and lock semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/layout_session.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_application_layout_session_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationLayoutSession *out_session)
{
    if (experience == NULL || out_session == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_session, 0, sizeof(*out_session));
    out_session->structure_size = sizeof(*out_session);
    out_session->experience = experience;
    out_session->selected_layout = umi_application_experience_layout_find(
        experience, experience->default_layout_id);
    return out_session->selected_layout != NULL ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_application_layout_session_select(
    UmiApplicationLayoutSession *session,
    const char *layout_id)
{
    const UmiExperienceLayoutDefinition *layout;
    if (session == NULL || session->experience == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->locked) return UMI_STATUS_PERMISSION_DENIED;
    layout = umi_application_experience_layout_find(session->experience, layout_id);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    if (layout != session->selected_layout) {
        session->selected_layout = layout;
        session->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_layout_session_set_locked(
    UmiApplicationLayoutSession *session,
    bool locked)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->locked != locked) {
        session->locked = locked;
        session->revision += 1U;
    }
    return UMI_STATUS_OK;
}

const UmiExperienceLayoutDefinition *umi_application_layout_session_current(
    const UmiApplicationLayoutSession *session)
{
    return session != NULL ? session->selected_layout : NULL;
}

/* Seed canonical UI workspace profiles from immutable experience layouts without overwriting user customisation. */
UmiStatus umi_application_layout_session_apply_to_profiles(
    const UmiApplicationLayoutSession *session,
    UmiUiWorkspaceProfileModel *profiles)
{
    size_t layout_index;
    UmiStatus status = UMI_STATUS_OK;
    if (session == NULL || session->experience == NULL || profiles == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    for (layout_index = 0U;
         layout_index < session->experience->layout_count && status == UMI_STATUS_OK;
         ++layout_index) {
        const UmiExperienceLayoutDefinition *layout = &session->experience->layouts[layout_index];
        UmiUiWorkspaceProfileSnapshot profile;
        size_t pane_index;

        if (layout->panel_count > UMI_UI_WORKSPACE_PROFILE_MAX_PANES)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        memset(&profile, 0, sizeof(profile));
        (void)snprintf(profile.profile_id, sizeof(profile.profile_id), "%s", layout->layout_id);
        (void)snprintf(profile.label, sizeof(profile.label), "%s", layout->title);
        (void)snprintf(profile.description, sizeof(profile.description), "%s", layout->description);
        profile.order = (int32_t)layout_index;
        profile.active = layout == session->selected_layout;
        profile.built_in = 0;
        profile.locked = 0;
        profile.pane_count = layout->panel_count;
        profile.editor_split_mode = UMI_UI_EDITOR_SPLIT_SINGLE;

        /* Convert immutable experience panel references into the existing
         * workspace-profile placement records used by every frontend. */
        for (pane_index = 0U; pane_index < layout->panel_count; ++pane_index) {
            const UmiExperiencePanelDefinition *panel =
                umi_application_experience_panel_find(session->experience,
                    layout->panel_ids[pane_index]);
            UmiUiPlacement placement;
            if (panel == NULL) return UMI_STATUS_NOT_FOUND;
            status = umi_ui_placement_parse(panel->default_region, &placement);
            if (status != UMI_STATUS_OK) break;
            (void)snprintf(profile.panes[pane_index].pane_id,
                           sizeof(profile.panes[pane_index].pane_id), "%s", panel->panel_id);
            profile.panes[pane_index].placement = placement;
            profile.panes[pane_index].order = (int32_t)pane_index;
            profile.panes[pane_index].visible = 1;
            if (placement == UMI_UI_PLACEMENT_LEFT) profile.sidebar_visible = 1;
            if (placement == UMI_UI_PLACEMENT_RIGHT) profile.auxiliary_sidebar_visible = 1;
            if (placement == UMI_UI_PLACEMENT_BOTTOM) profile.bottom_panel_visible = 1;
        }
        if (status == UMI_STATUS_OK) {
            UmiUiWorkspaceProfileSnapshot existing;
            /* Seed only missing experience profiles. Existing profiles may contain
             * user-adjusted placements and must not be overwritten by a sync. */
            UmiStatus find_status = umi_ui_workspace_profile_model_find(
                profiles, profile.profile_id, &existing);
            if (find_status == UMI_STATUS_NOT_FOUND)
                status = umi_ui_workspace_profile_model_upsert(profiles, &profile);
            else if (find_status != UMI_STATUS_OK)
                status = find_status;
        }
    }
    if (status == UMI_STATUS_OK && session->selected_layout != NULL) {
        status = umi_ui_workspace_profile_model_set_active(
            profiles, session->selected_layout->layout_id);
        if (status == UMI_STATUS_OK)
            status = umi_ui_workspace_profile_model_set_locked(
                profiles, session->selected_layout->layout_id, session->locked ? 1 : 0);
    }
    return status;
}
