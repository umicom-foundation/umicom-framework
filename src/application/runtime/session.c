/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/session.c
 *
 * PURPOSE:
 *   Implement bounded application session activation, layout selection and lock state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/session.h"

#include <string.h>

/* Provide the find active panel operation used by this module and its client applications. */
static int find_active_panel(const UmiApplicationSession *session, const char *panel_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || panel_id == NULL) return -1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->active_panel_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(session->active_panel_ids[index], panel_id) == 0)
            return (int)index;
    }
    return -1;
}

/*
 * Provide the load layout panels operation used by this module and its client
 * applications.
 */
static UmiStatus load_layout_panels(
    UmiApplicationSession *session,
    const UmiExperienceLayoutDefinition *layout)
{
    size_t index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (layout->panel_count > UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    session->active_panel_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < layout->panel_count; ++index)
        session->active_panel_ids[session->active_panel_count++] = layout->panel_ids[index];
    session->layout = layout;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Initialise application session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_application_session_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationSession *out_session)
{
    const UmiExperienceLayoutDefinition *layout;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_session == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_application_experience_validate(experience) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_session, 0, sizeof(*out_session));
    out_session->structure_size = sizeof(*out_session);
    out_session->experience = experience;
    layout = umi_application_experience_layout_find(
        experience, experience->default_layout_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    return load_layout_panels(out_session, layout);
}

/*
 * Provide the application session select layout operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_select_layout(
    UmiApplicationSession *session,
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
    if (session->layout_locked) return UMI_STATUS_PERMISSION_DENIED;
    layout = umi_application_experience_layout_find(session->experience, layout_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    return load_layout_panels(session, layout);
}

/*
 * Provide the application session activate panel operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_activate_panel(
    UmiApplicationSession *session,
    const char *panel_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || session->experience == NULL || panel_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_application_experience_panel_find(session->experience, panel_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (find_active_panel(session, panel_id) >= 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->active_panel_count >= UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    session->active_panel_ids[session->active_panel_count++] = panel_id;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the application session deactivate panel operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_deactivate_panel(
    UmiApplicationSession *session,
    const char *panel_id)
{
    int found;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    found = find_active_panel(session, panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (found < 0) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = (size_t)found; index + 1U < session->active_panel_count; ++index)
        session->active_panel_ids[index] = session->active_panel_ids[index + 1U];
    session->active_panel_count -= 1U;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the application session panel active operation used by this module and its
 * client applications.
 */
int umi_application_session_panel_active(
    const UmiApplicationSession *session,
    const char *panel_id)
{
    return find_active_panel(session, panel_id) >= 0;
}

/*
 * Provide the application session set layout locked operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_set_layout_locked(
    UmiApplicationSession *session,
    bool locked)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->layout_locked != locked) {
        session->layout_locked = locked;
        session->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Check that application session satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_application_session_validate(const UmiApplicationSession *session)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || session->structure_size != sizeof(*session) ||
        session->experience == NULL || session->layout == NULL ||
        session->active_panel_count > UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->active_panel_count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_application_experience_panel_find(
                session->experience, session->active_panel_ids[index]) == NULL)
            return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}
