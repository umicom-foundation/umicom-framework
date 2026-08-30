/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_preset.c
 *
 * PURPOSE:
 *   Exercise layout preset behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/layout_preset.h"
int main(void) {
    UmiUiMosaicLayoutPreset value;
    umi_ui_mosaic_layout_preset_init(&value);
    CHECK(umi_ui_mosaic_layout_preset_set(&value, "layout.layout_preset", "Layout Preset") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_layout_preset_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_layout_preset_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
