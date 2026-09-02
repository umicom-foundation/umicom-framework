/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_data_grid_surface.c
 *
 * PURPOSE:
 *   Implement the test data grid surface behavior for
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
#include "umicom/ui/workstation/data_grid_surface.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsDataGridSurface g;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_data_grid_surface_init(&g, "orders") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_data_grid_surface_add_column(&g, "symbol", "Symbol", 80) != UMI_STATUS_OK) return 2;
    umi_ws_data_grid_surface_set_rows(&g, 10U);
    /* Apply this branch only when its contract condition is satisfied. */
    if (g.column_count != 1U || g.row_count != 10U) return 3;
    puts("data grid surface: ok");
    return 0;
}
