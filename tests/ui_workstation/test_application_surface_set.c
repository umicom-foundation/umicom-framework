/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_application_surface_set.c
 *
 * PURPOSE:
 *   Implement the test application surface set behavior for
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
#include "umicom/ui/workstation/application_surface_set.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsApplicationSurfaceSet s;
    UmiWsSurfaceDescriptor d;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_application_surface_set_init(&s, "desktop", UMI_WS_DOMAIN_GENERIC) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_application_surface_set_add(&s, "studio.editor") != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ws_application_surface_set_contains(&s, "studio.editor")) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_surface_descriptor_init(&d, "trader.chart", "Chart", UMI_WS_DOMAIN_TRADER, UMI_WS_SURFACE_CHART) != UMI_STATUS_OK) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ws_application_surface_set_can_host(&s, &d)) return 5;
    puts("application surface set: ok");
    return 0;
}
