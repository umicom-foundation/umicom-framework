/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/panel_state.h
 *
 * PURPOSE:
 *   Define explicit docked, floating, auto-hidden and visibility state for reusable workstation panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_PANEL_STATE_H
#define UMICOM_UI_WORKSTATION_PANEL_STATE_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWsPanelMode {
    UMI_WS_PANEL_DOCKED = 1,
    UMI_WS_PANEL_TABBED = 2,
    UMI_WS_PANEL_FLOATING = 3,
    UMI_WS_PANEL_AUTO_HIDDEN = 4
} UmiWsPanelMode;

typedef struct UmiWsPanelState {
    char surface_id[UMI_UI_ID_CAPACITY];
    UmiWsPanelMode mode;
    UmiWsVisibility visibility;
    UmiWsDockRegion region;
    bool active;
    bool pinned;
    uint64_t revision;
} UmiWsPanelState;

UmiStatus umi_ws_panel_state_init(UmiWsPanelState *state, const char *surface_id, UmiWsDockRegion region);
UmiStatus umi_ws_panel_state_set_mode(UmiWsPanelState *state, UmiWsPanelMode mode);
void umi_ws_panel_state_set_active(UmiWsPanelState *state, bool active);

#ifdef __cplusplus
}
#endif

#endif
