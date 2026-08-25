/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/panel_state.c
 *
 * PURPOSE:
 *   Implement explicit docked, floating, auto-hidden and visibility state for reusable workstation panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/panel_state.h"

UmiStatus umi_ws_panel_state_init(UmiWsPanelState *state, const char *surface_id, UmiWsDockRegion region) {
    UmiStatus status;
    if (state == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (region < UMI_WS_DOCK_LEFT || region > UMI_WS_DOCK_FLOATING) return UMI_STATUS_INVALID_ARGUMENT;
    *state = (UmiWsPanelState){0};
    status = umi_ws_copy_text(state->surface_id, sizeof(state->surface_id), surface_id);
    if (status != UMI_STATUS_OK) return status;
    state->mode = (region == UMI_WS_DOCK_FLOATING) ? UMI_WS_PANEL_FLOATING : UMI_WS_PANEL_DOCKED;
    state->visibility = UMI_WS_VISIBILITY_VISIBLE;
    state->region = region;
    state->pinned = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_panel_state_set_mode(UmiWsPanelState *state, UmiWsPanelMode mode) {
    if (state == NULL || mode < UMI_WS_PANEL_DOCKED || mode > UMI_WS_PANEL_AUTO_HIDDEN) return UMI_STATUS_INVALID_ARGUMENT;
    state->mode = mode;
    if (mode == UMI_WS_PANEL_AUTO_HIDDEN) state->visibility = UMI_WS_VISIBILITY_AUTO_HIDE;
    else if (state->visibility == UMI_WS_VISIBILITY_AUTO_HIDE) state->visibility = UMI_WS_VISIBILITY_VISIBLE;
    if (mode == UMI_WS_PANEL_FLOATING) state->region = UMI_WS_DOCK_FLOATING;
    ++state->revision;
    return UMI_STATUS_OK;
}

void umi_ws_panel_state_set_active(UmiWsPanelState *state, bool active) {
    if (state != NULL) {
        state->active = active;
        ++state->revision;
    }
}
