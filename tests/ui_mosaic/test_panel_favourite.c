/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_favourite.c
 *
 * PURPOSE:
 *   Exercise panel favourite behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_favourite.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicPanelFavourite value;
    umi_ui_mosaic_panel_favourite_init(&value);
    CHECK(umi_ui_mosaic_panel_favourite_set(&value, "panel.panel_favourite", "Panel Favourite") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_favourite_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_favourite_rank(&value, 5U) == 16U);
    return 0;
}
