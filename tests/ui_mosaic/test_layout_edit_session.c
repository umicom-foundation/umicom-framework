/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_edit_session.c
 *
 * PURPOSE:
 *   Exercise layout edit session behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/layout_edit_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicLayoutEditSession value;
    umi_ui_mosaic_layout_edit_session_init(&value);
    CHECK(umi_ui_mosaic_layout_edit_session_set(&value, "layout.layout_edit_session", "Layout Edit Session") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_layout_edit_session_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_layout_edit_session_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
