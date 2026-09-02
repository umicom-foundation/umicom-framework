/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_mosaic_activation.c
 *
 * PURPOSE:
 *   Exercise mosaic activation behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/mosaic_activation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicMosaicActivation value;
    umi_ui_mosaic_mosaic_activation_init(&value);
    CHECK(umi_ui_mosaic_mosaic_activation_place(&value, "cell.mosaic_activation", "studio.editor", UMI_UI_MOSAIC_APP_STUDIO, 1U, 2U) == UMI_STATUS_OK);
    value.row_span = 2U; value.column_span = 3U;
    CHECK(umi_ui_mosaic_mosaic_activation_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_mosaic_activation_area(&value) == 6U);
    return 0;
}
