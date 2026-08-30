/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_search.c
 *
 * PURPOSE:
 *   Exercise layout search behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/layout_search.h"
int main(void) {
    UmiUiMosaicLayoutSearch value;
    umi_ui_mosaic_layout_search_init(&value);
    CHECK(umi_ui_mosaic_layout_search_set(&value, "layout.layout_search", "Layout Search") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_layout_search_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_layout_search_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
