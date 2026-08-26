/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/session.c
 *
 * PURPOSE:
 *   Implement bounded application session activation, layout selection and lock state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/session.h"

#include <string.h>

static int find_active_panel(const UmiApplicationSession *session, const char *panel_id)
{
    size_t index;
    if (session == NULL || panel_id == NULL) return -1;
    for (index = 0U; index < session->active_panel_count; ++index) {
        if (strcmp(session->active_panel_ids[index], panel_id) == 0)
            return (int)index;
    }
    return -1;
}

static UmiStatus load_layout_panels(
    UmiApplicationSession *session,
    const UmiExperienceLayoutDefinition *layout)
{
    size_t index;
    if (layout->panel_count > UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    session->active_panel_count = 0U;
    for (index = 0U; index < layout->panel_count; ++index)
        session->active_panel_ids[session->active_panel_count++] = layout->panel_ids[index];
    session->layout = layout;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_session_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationSession *out_session)
{
    const UmiExperienceLayoutDefinition *layout;
    if (experience == NULL || out_session == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_application_experience_validate(experience) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_session, 0, sizeof(*out_session));
    out_session->structure_size = sizeof(*out_session);
    out_session->experience = experience;
    layout = umi_application_experience_layout_find(
        experience, experience->default_layout_id);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    return load_layout_panels(out_session, layout);
}

UmiStatus umi_application_session_select_layout(
    UmiApplicationSession *session,
    const char *layout_id)
{
    const UmiExperienceLayoutDefinition *layout;
    if (session == NULL || session->experience == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->layout_locked) return UMI_STATUS_PERMISSION_DENIED;
    layout = umi_application_experience_layout_find(session->experience, layout_id);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    return load_layout_panels(session, layout);
}

UmiStatus umi_application_session_activate_panel(
    UmiApplicationSession *session,
    const char *panel_id)
{
    if (session == NULL || session->experience == NULL || panel_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_application_experience_panel_find(session->experience, panel_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (find_active_panel(session, panel_id) >= 0) return UMI_STATUS_ALREADY_EXISTS;
    if (session->active_panel_count >= UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    session->active_panel_ids[session->active_panel_count++] = panel_id;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_session_deactivate_panel(
    UmiApplicationSession *session,
    const char *panel_id)
{
    int found;
    size_t index;
    if (session == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    found = find_active_panel(session, panel_id);
    if (found < 0) return UMI_STATUS_NOT_FOUND;
    for (index = (size_t)found; index + 1U < session->active_panel_count; ++index)
        session->active_panel_ids[index] = session->active_panel_ids[index + 1U];
    session->active_panel_count -= 1U;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

int umi_application_session_panel_active(
    const UmiApplicationSession *session,
    const char *panel_id)
{
    return find_active_panel(session, panel_id) >= 0;
}

UmiStatus umi_application_session_set_layout_locked(
    UmiApplicationSession *session,
    bool locked)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->layout_locked != locked) {
        session->layout_locked = locked;
        session->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_session_validate(const UmiApplicationSession *session)
{
    size_t index;
    if (session == NULL || session->structure_size != sizeof(*session) ||
        session->experience == NULL || session->layout == NULL ||
        session->active_panel_count > UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < session->active_panel_count; ++index) {
        if (umi_application_experience_panel_find(
                session->experience, session->active_panel_ids[index]) == NULL)
            return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}
