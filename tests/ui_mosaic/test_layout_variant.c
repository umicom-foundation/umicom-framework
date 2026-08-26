/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_variant.c
 *
 * PURPOSE:
 *   Exercise layout variant behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/layout_variant.h"
int main(void) {
    UmiUiMosaicLayoutVariant value;
    umi_ui_mosaic_layout_variant_init(&value);
    CHECK(umi_ui_mosaic_layout_variant_set(&value, "layout.layout_variant", "Layout Variant") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_layout_variant_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_layout_variant_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
