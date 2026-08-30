/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_panel_state.c
 *
 * PURPOSE:
 *   Implement the test panel state behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/ui/workstation/panel_state.h"
int main(void) {
    UmiWsPanelState s;
    if (umi_ws_panel_state_init(&s, "ai.chat", UMI_WS_DOCK_RIGHT) != UMI_STATUS_OK) return 1;
    if (umi_ws_panel_state_set_mode(&s, UMI_WS_PANEL_AUTO_HIDDEN) != UMI_STATUS_OK) return 2;
    if (s.visibility != UMI_WS_VISIBILITY_AUTO_HIDE) return 3;
    umi_ws_panel_state_set_active(&s, true);
    if (!s.active) return 4;
    puts("panel state: ok");
    return 0;
}
