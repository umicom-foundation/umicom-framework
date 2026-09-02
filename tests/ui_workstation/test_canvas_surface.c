/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_canvas_surface.c
 *
 * PURPOSE:
 *   Implement the test canvas surface behavior for
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
#include "umicom/ui/workstation/canvas_surface.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsCanvasSurface c;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_canvas_surface_init(&c, "kitchen.plan") != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_canvas_surface_set_zoom(&c, 2.0) != 2.0) return 2;
    umi_ws_canvas_surface_pan(&c, 10.0, -5.0);
    umi_ws_canvas_surface_set_grid(&c, 20, true);
    /* Apply this branch only when its contract condition is satisfied. */
    if (c.pan_x != 10.0 || c.grid_size != 20) return 3;
    puts("canvas surface: ok");
    return 0;
}
