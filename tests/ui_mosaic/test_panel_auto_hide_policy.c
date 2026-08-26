/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_auto_hide_policy.c
 *
 * PURPOSE:
 *   Exercise panel auto hide policy behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_auto_hide_policy.h"
int main(void) {
    UmiUiMosaicPanelAutoHidePolicy value;
    umi_ui_mosaic_panel_auto_hide_policy_init(&value);
    CHECK(umi_ui_mosaic_panel_auto_hide_policy_set(&value, "panel.panel_auto_hide_policy", "Panel Auto Hide Policy") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_auto_hide_policy_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_auto_hide_policy_rank(&value, 5U) == 16U);
    return 0;
}
