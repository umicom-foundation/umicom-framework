/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_pinning.c
 *
 * PURPOSE:
 *   Exercise panel pinning behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_pinning.h"
int main(void) {
    UmiUiMosaicPanelPinning value;
    umi_ui_mosaic_panel_pinning_init(&value);
    CHECK(umi_ui_mosaic_panel_pinning_set(&value, "panel.panel_pinning", "Panel Pinning") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_pinning_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_pinning_rank(&value, 5U) == 16U);
    return 0;
}
