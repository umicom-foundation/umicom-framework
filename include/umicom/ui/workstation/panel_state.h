/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/panel_state.h
 *
 * PURPOSE:
 *   Define explicit docked, floating, auto-hidden and visibility state for reusable workstation panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_PANEL_STATE_H
#define UMICOM_UI_WORKSTATION_PANEL_STATE_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ws panel mode values accepted by this public contract.
 */
typedef enum UmiWsPanelMode {
    UMI_WS_PANEL_DOCKED = 1,
    UMI_WS_PANEL_TABBED = 2,
    UMI_WS_PANEL_FLOATING = 3,
    UMI_WS_PANEL_AUTO_HIDDEN = 4
} UmiWsPanelMode;

/**
 * Represent the ws panel state data shared with callers of this public contract.
 */
typedef struct UmiWsPanelState {
    char surface_id[UMI_UI_ID_CAPACITY];
    UmiWsPanelMode mode;
    UmiWsVisibility visibility;
    UmiWsDockRegion region;
    bool active;
    bool pinned;
    uint64_t revision;
} UmiWsPanelState;

/**
 * Initialise ws panel state from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_panel_state_init(UmiWsPanelState *state, const char *surface_id, UmiWsDockRegion region);
/**
 * Provide the ws panel state set mode operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_panel_state_set_mode(UmiWsPanelState *state, UmiWsPanelMode mode);
/**
 * Provide the ws panel state set active operation used by this module and its client
 * applications.
 */
void umi_ws_panel_state_set_active(UmiWsPanelState *state, bool active);

#ifdef __cplusplus
}
#endif

#endif
