/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/auto_hide.c
 *
 * PURPOSE:
 *   Implement auto-hide edge-rail state without destroying the stored layout graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/auto_hide.h"

/* Check that ws auto hide edge satisfies its contract before another service relies on it. */
bool umi_ws_auto_hide_edge_valid(UmiWsDockRegion edge) {
    return edge == UMI_WS_DOCK_LEFT || edge == UMI_WS_DOCK_RIGHT ||
           edge == UMI_WS_DOCK_TOP || edge == UMI_WS_DOCK_BOTTOM;
}

/*
 * Initialise ws auto hide from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ws_auto_hide_init(UmiWsAutoHideState *state, const char *surface_id, UmiWsDockRegion edge) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || !umi_ws_id_valid(surface_id) || !umi_ws_auto_hide_edge_valid(edge)) return UMI_STATUS_INVALID_ARGUMENT;
    *state = (UmiWsAutoHideState){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(state->surface_id, sizeof(state->surface_id), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    state->edge = edge;
    state->enabled = true;
    state->reveal_extent = 320;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws auto hide set revealed operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_auto_hide_set_revealed(UmiWsAutoHideState *state, bool revealed) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || !state->enabled) return UMI_STATUS_INVALID_STATE;
    state->revealed = revealed;
    return UMI_STATUS_OK;
}
