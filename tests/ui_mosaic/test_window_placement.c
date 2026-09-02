/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_window_placement.c
 *
 * PURPOSE:
 *   Exercise window placement behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/window_placement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicWindowPlacement value;
    umi_ui_mosaic_window_placement_init(&value);
    CHECK(umi_ui_mosaic_window_placement_set(&value, "layout.window_placement", "Window Placement") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_window_placement_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_window_placement_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
