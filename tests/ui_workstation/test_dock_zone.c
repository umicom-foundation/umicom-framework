/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_dock_zone.c
 *
 * PURPOSE:
 *   Implement the test dock zone behavior for
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
#include "umicom/ui/workstation/dock_zone.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsDockZone z;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_dock_zone_classify(0.05, 0.5, 0.2) != UMI_WS_DOCK_LEFT) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_dock_zone_classify(0.5, 0.5, 0.2) != UMI_WS_DOCK_CENTRE) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_dock_zone_init(&z, UMI_WS_DOCK_BOTTOM, 0.25) != UMI_STATUS_OK) return 3;
    puts("dock zone: ok");
    return 0;
}
