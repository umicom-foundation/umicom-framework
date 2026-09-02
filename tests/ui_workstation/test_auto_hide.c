/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_auto_hide.c
 *
 * PURPOSE:
 *   Implement the test auto hide behavior for
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
#include "umicom/ui/workstation/auto_hide.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsAutoHideState s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_auto_hide_init(&s, "outline", UMI_WS_DOCK_LEFT) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_auto_hide_set_revealed(&s, true) != UMI_STATUS_OK || !s.revealed) return 2;
    /* Apply this operation only while the related capability or state is available. */
    if (umi_ws_auto_hide_edge_valid(UMI_WS_DOCK_CENTRE)) return 3;
    puts("auto hide: ok");
    return 0;
}
