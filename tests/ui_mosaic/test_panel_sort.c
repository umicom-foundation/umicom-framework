/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_sort.c
 *
 * PURPOSE:
 *   Exercise panel sort behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_sort.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicPanelSort value;
    umi_ui_mosaic_panel_sort_init(&value);
    CHECK(umi_ui_mosaic_panel_sort_set(&value, "panel.panel_sort", "Panel Sort") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_sort_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_sort_rank(&value, 5U) == 16U);
    return 0;
}
