/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_perspective_switch.c
 *
 * PURPOSE:
 *   Exercise perspective switch behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/perspective_switch.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicPerspectiveSwitch value;
    umi_ui_mosaic_perspective_switch_init(&value);
    CHECK(umi_ui_mosaic_perspective_switch_set(&value, "perspective.perspective_switch", "Perspective Switch", "layout.default", UMI_UI_MOSAIC_APP_TMS) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_perspective_switch_validate(&value) == UMI_STATUS_OK);
    return 0;
}
