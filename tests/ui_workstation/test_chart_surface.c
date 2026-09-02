/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_chart_surface.c
 *
 * PURPOSE:
 *   Implement the test chart surface behavior for
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
#include "umicom/ui/workstation/chart_surface.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsChartSurface c;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_chart_surface_init(&c, "chart.main", "ES · 1m") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_chart_surface_set_context_group(&c, "group.1") != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_chart_surface_set_interval(&c, 300) != UMI_STATUS_OK) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_chart_surface_set_zoom(&c, 50.0) != 20.0) return 4;
    puts("chart surface: ok");
    return 0;
}
