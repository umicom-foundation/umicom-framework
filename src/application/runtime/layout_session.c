/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/layout_session.c
 *
 * PURPOSE:
 *   Implement deterministic layout selection and lock semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/layout_session.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise application layout session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_layout_session_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationLayoutSession *out_session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_session == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_session, 0, sizeof(*out_session));
    out_session->structure_size = sizeof(*out_session);
    out_session->experience = experience;
    out_session->selected_layout = umi_application_experience_layout_find(
        experience, experience->default_layout_id);
    return out_session->selected_layout != NULL ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the application layout session select operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_layout_session_select(
    UmiApplicationLayoutSession *session,
    const char *layout_id)
{
    const UmiExperienceLayoutDefinition *layout;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || session->experience == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->locked) return UMI_STATUS_PERMISSION_DENIED;
    layout = umi_application_experience_layout_find(session->experience, layout_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (layout != session->selected_layout) {
        session->selected_layout = layout;
        session->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application layout session set locked operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_layout_session_set_locked(
    UmiApplicationLayoutSession *session,
    bool locked)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->locked != locked) {
        session->locked = locked;
        session->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application layout session current operation used by this module and its
 * client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || session->experience == NULL || profiles == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Visit each bounded item once so every record receives the same rule. */
    for (layout_index = 0U;
         layout_index < session->experience->layout_count && status == UMI_STATUS_OK;
         ++layout_index) {
        const UmiExperienceLayoutDefinition *layout = &session->experience->layouts[layout_index];
        UmiUiWorkspaceProfileSnapshot profile;
        size_t pane_index;

        /* Apply this branch only when its contract condition is satisfied. */
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
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (panel == NULL) return UMI_STATUS_NOT_FOUND;
            status = umi_ui_placement_parse(panel->default_region, &placement);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) break;
            (void)snprintf(profile.panes[pane_index].pane_id,
                           sizeof(profile.panes[pane_index].pane_id), "%s", panel->panel_id);
            profile.panes[pane_index].placement = placement;
            profile.panes[pane_index].order = (int32_t)pane_index;
            profile.panes[pane_index].visible = 1;
            /* Apply this operation only while the related capability or state is available. */
            if (placement == UMI_UI_PLACEMENT_LEFT) profile.sidebar_visible = 1;
            /* Apply this operation only while the related capability or state is available. */
            if (placement == UMI_UI_PLACEMENT_RIGHT) profile.auxiliary_sidebar_visible = 1;
            /* Apply this operation only while the related capability or state is available. */
            if (placement == UMI_UI_PLACEMENT_BOTTOM) profile.bottom_panel_visible = 1;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            UmiUiWorkspaceProfileSnapshot existing;
            /* Seed only missing experience profiles. Existing profiles may contain
             * user-adjusted placements and must not be overwritten by a sync. */
            UmiStatus find_status = umi_ui_workspace_profile_model_find(
                profiles, profile.profile_id, &existing);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (find_status == UMI_STATUS_NOT_FOUND)
                status = umi_ui_workspace_profile_model_upsert(profiles, &profile);
            else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (find_status != UMI_STATUS_OK)
                status = find_status;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && session->selected_layout != NULL) {
        status = umi_ui_workspace_profile_model_set_active(
            profiles, session->selected_layout->layout_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            status = umi_ui_workspace_profile_model_set_locked(
                profiles, session->selected_layout->layout_id, session->locked ? 1 : 0);
    }
    return status;
}
