/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/session_view.c
 *
 * PURPOSE:
 *   Project current session state for status bars, inspectors and operations views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application_ui/session_view.h"

UmiStatus umi_application_ui_session_row(
    const UmiApplicationSession *session,
    UmiApplicationUiSessionRow *out_row)
{
    if (session == NULL || out_row == NULL ||
        umi_application_session_validate(session) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    out_row->application_id = session->experience->application_id;
    out_row->layout_id = session->layout->layout_id;
    out_row->active_panel_count = session->active_panel_count;
    out_row->layout_locked = session->layout_locked ? 1 : 0;
    out_row->revision = session->revision;
    return UMI_STATUS_OK;
}
