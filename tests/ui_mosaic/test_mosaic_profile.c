/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_mosaic_profile.c
 *
 * PURPOSE:
 *   Exercise mosaic profile behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/mosaic_profile.h"
int main(void) {
    UmiUiMosaicMosaicProfile value;
    umi_ui_mosaic_mosaic_profile_init(&value);
    CHECK(umi_ui_mosaic_mosaic_profile_place(&value, "cell.mosaic_profile", "studio.editor", UMI_UI_MOSAIC_APP_STUDIO, 1U, 2U) == UMI_STATUS_OK);
    value.row_span = 2U; value.column_span = 3U;
    CHECK(umi_ui_mosaic_mosaic_profile_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_mosaic_profile_area(&value) == 6U);
    return 0;
}
