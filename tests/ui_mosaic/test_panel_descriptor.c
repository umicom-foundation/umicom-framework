/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_descriptor.c
 *
 * PURPOSE:
 *   Exercise panel descriptor behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_descriptor.h"
int main(void) {
    UmiUiMosaicPanelDescriptor value;
    umi_ui_mosaic_panel_descriptor_init(&value);
    CHECK(umi_ui_mosaic_panel_descriptor_set(&value, "panel.panel_descriptor", "Panel Descriptor") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_descriptor_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_descriptor_rank(&value, 5U) == 16U);
    return 0;
}
