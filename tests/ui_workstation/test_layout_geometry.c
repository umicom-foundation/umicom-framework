/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_geometry.c
 *
 * PURPOSE:
 *   Implement the test layout geometry behavior for
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
#include "umicom/ui/workstation/layout_geometry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiRect w = {0,0,1000,700};
    UmiUiRect r = {-50,-10,1200,800};
    UmiUiRect c = umi_ws_layout_geometry_clamp(r, w);
    UmiUiRect s = umi_ws_layout_geometry_scale((UmiUiRect){10,20,100,200}, 100, 150);
    /* Apply this branch only when its contract condition is satisfied. */
    if (c.x != 0 || c.y != 0 || c.width != 1000 || c.height != 700) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (s.width != 150 || s.height != 300) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_layout_geometry_area((UmiUiRect){0,0,10,20}) != 200) return 3;
    puts("layout geometry: ok");
    return 0;
}
