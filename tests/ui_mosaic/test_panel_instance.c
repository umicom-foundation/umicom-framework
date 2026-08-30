/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_instance.c
 *
 * PURPOSE:
 *   Exercise panel instance behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_instance.h"
int main(void) {
    UmiUiMosaicPanelInstance value;
    umi_ui_mosaic_panel_instance_init(&value);
    CHECK(umi_ui_mosaic_panel_instance_set(&value, "panel.panel_instance", "Panel Instance") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_instance_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_instance_rank(&value, 5U) == 16U);
    return 0;
}
