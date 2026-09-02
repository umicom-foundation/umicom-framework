/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_edit_session.c
 *
 * PURPOSE:
 *   Implement transactional layout edit state for unlock, preview, commit and cancel workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/layout_edit_session.h"

/*
 * Provide the ws layout edit session begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_layout_edit_session_begin(UmiWsLayoutEditSession *session, const UmiWsLayoutTemplate *layout) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || layout == NULL || !umi_ws_id_valid(layout->layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *session = (UmiWsLayoutEditSession){0};
    session->original = *layout;
    session->working = *layout;
    session->active = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws layout edit session working operation used by this module and its client
 * applications.
 */
UmiWsLayoutTemplate *umi_ws_layout_edit_session_working(UmiWsLayoutEditSession *session) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || !session->active) return NULL;
    return &session->working;
}

/*
 * Provide the ws layout edit session mark dirty operation used by this module and its
 * client applications.
 */
void umi_ws_layout_edit_session_mark_dirty(UmiWsLayoutEditSession *session) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session != NULL && session->active) session->dirty = true;
}

/*
 * Provide the ws layout edit session commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_layout_edit_session_commit(UmiWsLayoutEditSession *session, UmiWsLayoutTemplate *out_layout) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_layout == NULL || !session->active) return UMI_STATUS_INVALID_STATE;
    *out_layout = session->working;
    session->active = false;
    session->dirty = false;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws layout edit session cancel operation used by this module and its client
 * applications.
 */
void umi_ws_layout_edit_session_cancel(UmiWsLayoutEditSession *session) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session != NULL) {
        session->working = session->original;
        session->active = false;
        session->dirty = false;
    }
}
