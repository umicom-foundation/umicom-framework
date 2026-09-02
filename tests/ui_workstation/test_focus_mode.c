/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_focus_mode.c
 *
 * PURPOSE:
 *   Implement the test focus mode behavior for
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
#include "umicom/ui/workstation/focus_mode.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsFocusMode m;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_focus_mode_enter(&m, "editor", 4U) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ws_focus_mode_should_hide(&m, "terminal")) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_focus_mode_should_hide(&m, "editor")) return 3;
    umi_ws_focus_mode_exit(&m);
    /* Apply this operation only while the related capability or state is available. */
    if (m.active) return 4;
    puts("focus mode: ok");
    return 0;
}
