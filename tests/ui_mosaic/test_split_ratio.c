/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_split_ratio.c
 *
 * PURPOSE:
 *   Exercise split ratio behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/split_ratio.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicSplitRatio value;
    umi_ui_mosaic_split_ratio_init(&value);
    CHECK(umi_ui_mosaic_split_ratio_set(&value, "layout.split_ratio", "Split Ratio") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_split_ratio_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_split_ratio_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
