/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_category.c
 *
 * PURPOSE:
 *   Exercise panel category behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_category.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicPanelCategory value;
    umi_ui_mosaic_panel_category_init(&value);
    CHECK(umi_ui_mosaic_panel_category_set(&value, "panel.panel_category", "Panel Category") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_category_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_category_rank(&value, 5U) == 16U);
    return 0;
}
