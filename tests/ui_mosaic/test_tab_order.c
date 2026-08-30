/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_tab_order.c
 *
 * PURPOSE:
 *   Exercise tab order behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/tab_order.h"
int main(void) {
    UmiUiMosaicTabOrder value;
    umi_ui_mosaic_tab_order_init(&value);
    CHECK(umi_ui_mosaic_tab_order_set(&value, "layout.tab_order", "Tab Order") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_tab_order_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_tab_order_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
