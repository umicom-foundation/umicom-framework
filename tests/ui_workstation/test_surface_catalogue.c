/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_surface_catalogue.c
 *
 * PURPOSE:
 *   Implement the test surface catalogue behavior for
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
#include "umicom/ui/workstation/surface_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsSurfaceCatalogue c;
    UmiWsSurfaceDescriptor d;
    umi_ws_surface_catalogue_init(&c);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_surface_descriptor_init(&d, "trader.chart", "Chart", UMI_WS_DOMAIN_TRADER, UMI_WS_SURFACE_CHART) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_surface_catalogue_upsert(&c, &d) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ws_surface_catalogue_find(&c, "trader.chart") == NULL) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_surface_catalogue_count_domain(&c, UMI_WS_DOMAIN_TRADER) != 1U) return 4;
    puts("surface catalogue: ok");
    return 0;
}
