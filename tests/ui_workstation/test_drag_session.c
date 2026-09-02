/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_drag_session.c
 *
 * PURPOSE:
 *   Implement the test drag session behavior for
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
#include "umicom/ui/workstation/drag_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsDragSession s;
    UmiUiPoint a = {10,20};
    UmiUiPoint b = {25,5};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_drag_session_begin(&s, "project", a) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_drag_session_move(&s, b, UMI_WS_DOCK_LEFT) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_drag_session_distance(&s) != 30) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_drag_session_commit(&s) != UMI_STATUS_OK || !s.committed) return 4;
    puts("drag session: ok");
    return 0;
}
