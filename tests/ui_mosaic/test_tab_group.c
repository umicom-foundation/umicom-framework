/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_tab_group.c
 *
 * PURPOSE:
 *   Exercise tab group behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/tab_group.h"
int main(void) {
    UmiUiMosaicTabGroup value;
    umi_ui_mosaic_tab_group_init(&value);
    CHECK(umi_ui_mosaic_tab_group_set(&value, "layout.tab_group", "Tab Group") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_tab_group_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_tab_group_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
