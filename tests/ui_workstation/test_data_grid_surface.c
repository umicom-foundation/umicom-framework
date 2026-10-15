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
#include "../value_archive/construction_cases.h"
#include "umicom/ui/workstation/data_grid_surface.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted data grid surface value. */
static int UmiWsDataGridSurfaceConstructionCases(void)
{
    UmiWsDataGridSurface *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_data_grid_surface_init(value, "orders") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->grid_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_data_grid_surface_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_data_grid_surface_init(value, value->grid_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->grid_id, "orders") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsDataGridSurfaceConstructionCases() != 0) return 1;

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
