/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/panel_state.c
 *
 * PURPOSE:
 *   Implement explicit docked, floating, auto-hidden and visibility state for reusable workstation panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/panel_state.h"

/*
 * Initialise ws panel state from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_panel_state_init(UmiWsPanelState *state, const char *surface_id, UmiWsDockRegion region) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (region < UMI_WS_DOCK_LEFT || region > UMI_WS_DOCK_FLOATING) return UMI_STATUS_INVALID_ARGUMENT;
    *state = (UmiWsPanelState){0};
    status = umi_ws_copy_text(state->surface_id, sizeof(state->surface_id), surface_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    state->mode = (region == UMI_WS_DOCK_FLOATING) ? UMI_WS_PANEL_FLOATING : UMI_WS_PANEL_DOCKED;
    state->visibility = UMI_WS_VISIBILITY_VISIBLE;
    state->region = region;
    state->pinned = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws panel state set mode operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_panel_state_set_mode(UmiWsPanelState *state, UmiWsPanelMode mode) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || mode < UMI_WS_PANEL_DOCKED || mode > UMI_WS_PANEL_AUTO_HIDDEN) return UMI_STATUS_INVALID_ARGUMENT;
    state->mode = mode;
    /* Apply this branch only when its contract condition is satisfied. */
    if (mode == UMI_WS_PANEL_AUTO_HIDDEN) state->visibility = UMI_WS_VISIBILITY_AUTO_HIDE;
    else /* Apply this operation only while the related capability or state is available. */ if (state->visibility == UMI_WS_VISIBILITY_AUTO_HIDE) state->visibility = UMI_WS_VISIBILITY_VISIBLE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (mode == UMI_WS_PANEL_FLOATING) state->region = UMI_WS_DOCK_FLOATING;
    ++state->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws panel state set active operation used by this module and its client
 * applications.
 */
void umi_ws_panel_state_set_active(UmiWsPanelState *state, bool active) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state != NULL) {
        state->active = active;
        ++state->revision;
    }
}
