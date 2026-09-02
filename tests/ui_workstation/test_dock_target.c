/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_dock_target.c
 *
 * PURPOSE:
 *   Implement the test dock target behavior for
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
#include "umicom/ui/workstation/dock_target.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsDockTarget t;
    UmiUiRect r = {0,0,100,100};
    UmiUiPoint p = {50,50};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_dock_target_init(&t, "centre", UMI_WS_DOCK_CENTRE, r) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ws_dock_target_contains(&t, p)) return 2;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_dock_target_score(&t, p) <= 0) return 3;
    puts("dock target: ok");
    return 0;
}
