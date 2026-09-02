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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsPanelState s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_panel_state_init(&s, "ai.chat", UMI_WS_DOCK_RIGHT) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_panel_state_set_mode(&s, UMI_WS_PANEL_AUTO_HIDDEN) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (s.visibility != UMI_WS_VISIBILITY_AUTO_HIDE) return 3;
    umi_ws_panel_state_set_active(&s, true);
    /* Apply this operation only while the related capability or state is available. */
    if (!s.active) return 4;
    puts("panel state: ok");
    return 0;
}
