/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_filter.c
 *
 * PURPOSE:
 *   Exercise panel filter behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_filter.h"
int main(void) {
    UmiUiMosaicPanelFilter value;
    umi_ui_mosaic_panel_filter_init(&value);
    CHECK(umi_ui_mosaic_panel_filter_set(&value, "panel.panel_filter", "Panel Filter") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_filter_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_filter_rank(&value, 5U) == 16U);
    return 0;
}
