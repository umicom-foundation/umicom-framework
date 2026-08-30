/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_floating_panel.c
 *
 * PURPOSE:
 *   Exercise floating panel behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/floating_panel.h"
int main(void) {
    UmiUiMosaicFloatingPanel value;
    umi_ui_mosaic_floating_panel_init(&value);
    CHECK(umi_ui_mosaic_floating_panel_set(&value, "layout.floating_panel", "Floating Panel") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_floating_panel_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_floating_panel_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
