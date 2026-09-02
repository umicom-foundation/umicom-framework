/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_dock_validation.c
 *
 * PURPOSE:
 *   Exercise dock validation behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/dock_validation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicDockValidation value;
    umi_ui_mosaic_dock_validation_init(&value);
    CHECK(umi_ui_mosaic_dock_validation_set(&value, "panel.source", "panel.target", UMI_UI_MOSAIC_DOCK_CENTRE) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_validation_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_validation_is_centre(&value) == 1);
    return 0;
}
