/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_edit_session.c
 *
 * PURPOSE:
 *   Implement transactional layout edit state for unlock, preview, commit and cancel workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/layout_edit_session.h"

UmiStatus umi_ws_layout_edit_session_begin(UmiWsLayoutEditSession *session, const UmiWsLayoutTemplate *layout) {
    if (session == NULL || layout == NULL || !umi_ws_id_valid(layout->layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *session = (UmiWsLayoutEditSession){0};
    session->original = *layout;
    session->working = *layout;
    session->active = true;
    return UMI_STATUS_OK;
}

UmiWsLayoutTemplate *umi_ws_layout_edit_session_working(UmiWsLayoutEditSession *session) {
    if (session == NULL || !session->active) return NULL;
    return &session->working;
}

void umi_ws_layout_edit_session_mark_dirty(UmiWsLayoutEditSession *session) {
    if (session != NULL && session->active) session->dirty = true;
}

UmiStatus umi_ws_layout_edit_session_commit(UmiWsLayoutEditSession *session, UmiWsLayoutTemplate *out_layout) {
    if (session == NULL || out_layout == NULL || !session->active) return UMI_STATUS_INVALID_STATE;
    *out_layout = session->working;
    session->active = false;
    session->dirty = false;
    return UMI_STATUS_OK;
}

void umi_ws_layout_edit_session_cancel(UmiWsLayoutEditSession *session) {
    if (session != NULL) {
        session->working = session->original;
        session->active = false;
        session->dirty = false;
    }
}
