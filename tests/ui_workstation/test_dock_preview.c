/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_dock_preview.c
 *
 * PURPOSE:
 *   Implement the test dock preview behavior for
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
#include "umicom/ui/workstation/dock_preview.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiRect r = {0,0,1000,800};
    UmiWsDockPreview p = umi_ws_dock_preview_compute(r, UMI_WS_DOCK_RIGHT, 0.25);
    /* Apply this operation only while the related capability or state is available. */
    if (!p.visible || p.bounds.width != 250 || p.bounds.x != 750) return 1;
    puts("dock preview: ok");
    return 0;
}
