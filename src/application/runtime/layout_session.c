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
