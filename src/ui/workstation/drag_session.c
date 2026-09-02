/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/drag_session.c
 *
 * PURPOSE:
 *   Implement explicit drag-session state shared by GTK4 pointer DnD and keyboard panel movement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/drag_session.h"

/*
 * Provide the ws drag session begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_drag_session_begin(UmiWsDragSession *session, const char *surface_id, UmiUiPoint start) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *session = (UmiWsDragSession){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(session->surface_id, sizeof(session->surface_id), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    session->start = start;
    session->current = start;
    session->proposed_region = UMI_WS_DOCK_CENTRE;
    session->active = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws drag session move operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_drag_session_move(UmiWsDragSession *session, UmiUiPoint current, UmiWsDockRegion proposed_region) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || !session->active) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (proposed_region < UMI_WS_DOCK_LEFT || proposed_region > UMI_WS_DOCK_FLOATING) return UMI_STATUS_INVALID_ARGUMENT;
    session->current = current;
    session->proposed_region = proposed_region;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws drag session commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_drag_session_commit(UmiWsDragSession *session) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || !session->active) return UMI_STATUS_INVALID_STATE;
    session->active = false;
    session->committed = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws drag session cancel operation used by this module and its client
 * applications.
 */
void umi_ws_drag_session_cancel(UmiWsDragSession *session) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session != NULL) {
        session->active = false;
        session->committed = false;
    }
}

/*
 * Provide the ws drag session distance operation used by this module and its client
 * applications.
 */
int32_t umi_ws_drag_session_distance(const UmiWsDragSession *session) {
    int32_t dx;
    int32_t dy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return 0;
    dx = session->current.x - session->start.x;
    dy = session->current.y - session->start.y;
    /* Apply this branch only when its contract condition is satisfied. */
    if (dx < 0) dx = -dx;
    /* Apply this branch only when its contract condition is satisfied. */
    if (dy < 0) dy = -dy;
    return dx + dy;
}
