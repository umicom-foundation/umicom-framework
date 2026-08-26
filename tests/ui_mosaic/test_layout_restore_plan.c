/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_restore_plan.c
 *
 * PURPOSE:
 *   Exercise layout restore plan behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/layout_restore_plan.h"
int main(void) {
    UmiUiMosaicLayoutRestorePlan value;
    umi_ui_mosaic_layout_restore_plan_init(&value);
    CHECK(umi_ui_mosaic_layout_restore_plan_set(&value, "layout.layout_restore_plan", "Layout Restore Plan") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_layout_restore_plan_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_layout_restore_plan_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
