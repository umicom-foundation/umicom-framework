/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_application_panel_pack.c
 *
 * PURPOSE:
 *   Exercise application panel pack behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/application_panel_pack.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicApplicationPanelPack value;
    umi_ui_mosaic_application_panel_pack_init(&value);
    CHECK(umi_ui_mosaic_application_panel_pack_place(&value, "cell.application_panel_pack", "studio.editor", UMI_UI_MOSAIC_APP_STUDIO, 1U, 2U) == UMI_STATUS_OK);
    value.row_span = 2U; value.column_span = 3U;
    CHECK(umi_ui_mosaic_application_panel_pack_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_application_panel_pack_area(&value) == 6U);
    return 0;
}
