/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_split_layout.c
 *
 * PURPOSE:
 *   Exercise split layout behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/split_layout.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicSplitLayout value;
    umi_ui_mosaic_split_layout_init(&value);
    CHECK(umi_ui_mosaic_split_layout_set(&value, "layout.split_layout", "Split Layout") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_split_layout_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_split_layout_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
