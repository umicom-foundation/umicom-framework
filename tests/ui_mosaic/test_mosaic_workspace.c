/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_mosaic_workspace.c
 *
 * PURPOSE:
 *   Exercise mosaic workspace behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/mosaic_workspace.h"
int main(void) {
    UmiUiMosaicMosaicWorkspace value;
    umi_ui_mosaic_mosaic_workspace_init(&value);
    CHECK(umi_ui_mosaic_mosaic_workspace_place(&value, "cell.mosaic_workspace", "studio.editor", UMI_UI_MOSAIC_APP_STUDIO, 1U, 2U) == UMI_STATUS_OK);
    value.row_span = 2U; value.column_span = 3U;
    CHECK(umi_ui_mosaic_mosaic_workspace_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_mosaic_workspace_area(&value) == 6U);
    return 0;
}
