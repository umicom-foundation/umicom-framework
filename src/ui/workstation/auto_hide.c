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
#include "../../base/value_construction_internal.h"

/* Check that ws auto hide edge satisfies its contract before another service relies on it. */
bool umi_ws_auto_hide_edge_valid(UmiWsDockRegion edge) {
    return edge == UMI_WS_DOCK_LEFT || edge == UMI_WS_DOCK_RIGHT ||
           edge == UMI_WS_DOCK_TOP || edge == UMI_WS_DOCK_BOTTOM;
}

/*
 * Initialise ws auto hide from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
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
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_ws_auto_hide_init_build_value(UmiWsAutoHideState *state, const char *surface_id, UmiWsDockRegion edge) {
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

UMI_DEFINE_STAGED_VALUE_INIT(umi_ws_auto_hide_init, UmiWsAutoHideState, state,
    (UmiWsAutoHideState *state, const char *surface_id, UmiWsDockRegion edge),
    umi_ws_auto_hide_init_build_value(candidate, surface_id, edge))


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
