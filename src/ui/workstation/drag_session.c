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

UmiStatus umi_ws_drag_session_begin(UmiWsDragSession *session, const char *surface_id, UmiUiPoint start) {
    if (session == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *session = (UmiWsDragSession){0};
    if (umi_ws_copy_text(session->surface_id, sizeof(session->surface_id), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    session->start = start;
    session->current = start;
    session->proposed_region = UMI_WS_DOCK_CENTRE;
    session->active = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_drag_session_move(UmiWsDragSession *session, UmiUiPoint current, UmiWsDockRegion proposed_region) {
    if (session == NULL || !session->active) return UMI_STATUS_INVALID_STATE;
    if (proposed_region < UMI_WS_DOCK_LEFT || proposed_region > UMI_WS_DOCK_FLOATING) return UMI_STATUS_INVALID_ARGUMENT;
    session->current = current;
    session->proposed_region = proposed_region;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_drag_session_commit(UmiWsDragSession *session) {
    if (session == NULL || !session->active) return UMI_STATUS_INVALID_STATE;
    session->active = false;
    session->committed = true;
    return UMI_STATUS_OK;
}

void umi_ws_drag_session_cancel(UmiWsDragSession *session) {
    if (session != NULL) {
        session->active = false;
        session->committed = false;
    }
}

int32_t umi_ws_drag_session_distance(const UmiWsDragSession *session) {
    int32_t dx;
    int32_t dy;
    if (session == NULL) return 0;
    dx = session->current.x - session->start.x;
    dy = session->current.y - session->start.y;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return dx + dy;
}
