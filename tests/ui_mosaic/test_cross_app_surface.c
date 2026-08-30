/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_cross_app_surface.c
 *
 * PURPOSE:
 *   Exercise cross app surface behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/cross_app_surface.h"
int main(void) {
    UmiUiMosaicCrossAppSurface value;
    umi_ui_mosaic_cross_app_surface_init(&value);
    CHECK(umi_ui_mosaic_cross_app_surface_place(&value, "cell.cross_app_surface", "studio.editor", UMI_UI_MOSAIC_APP_STUDIO, 1U, 2U) == UMI_STATUS_OK);
    value.row_span = 2U; value.column_span = 3U;
    CHECK(umi_ui_mosaic_cross_app_surface_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_cross_app_surface_area(&value) == 6U);
    return 0;
}
