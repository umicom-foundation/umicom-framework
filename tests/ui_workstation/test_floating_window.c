/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_floating_window.c
 *
 * PURPOSE:
 *   Implement the test floating window behavior for
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
#include "umicom/ui/workstation/floating_window.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsFloatingWindow w;
    UmiUiRect r = {100,100,800,600};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_floating_window_init(&w, "float.1", "chart.2", r) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_floating_window_assign_monitor(&w, "monitor.2") != UMI_STATUS_OK) return 2;
    umi_ws_floating_window_set_maximised(&w, true);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!w.maximised) return 3;
    puts("floating window: ok");
    return 0;
}
