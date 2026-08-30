/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_recent.c
 *
 * PURPOSE:
 *   Exercise panel recent behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_recent.h"
int main(void) {
    UmiUiMosaicPanelRecent value;
    umi_ui_mosaic_panel_recent_init(&value);
    CHECK(umi_ui_mosaic_panel_recent_set(&value, "panel.panel_recent", "Panel Recent") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_recent_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_recent_rank(&value, 5U) == 16U);
    return 0;
}
