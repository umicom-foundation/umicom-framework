/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/auto_hide.c
 *
 * PURPOSE:
 *   Implement auto-hide edge-rail state without destroying the stored layout graph.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/auto_hide.h"

bool umi_ws_auto_hide_edge_valid(UmiWsDockRegion edge) {
    return edge == UMI_WS_DOCK_LEFT || edge == UMI_WS_DOCK_RIGHT ||
           edge == UMI_WS_DOCK_TOP || edge == UMI_WS_DOCK_BOTTOM;
}

UmiStatus umi_ws_auto_hide_init(UmiWsAutoHideState *state, const char *surface_id, UmiWsDockRegion edge) {
    if (state == NULL || !umi_ws_id_valid(surface_id) || !umi_ws_auto_hide_edge_valid(edge)) return UMI_STATUS_INVALID_ARGUMENT;
    *state = (UmiWsAutoHideState){0};
    if (umi_ws_copy_text(state->surface_id, sizeof(state->surface_id), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    state->edge = edge;
    state->enabled = true;
    state->reveal_extent = 320;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_auto_hide_set_revealed(UmiWsAutoHideState *state, bool revealed) {
    if (state == NULL || !state->enabled) return UMI_STATUS_INVALID_STATE;
    state->revealed = revealed;
    return UMI_STATUS_OK;
}
