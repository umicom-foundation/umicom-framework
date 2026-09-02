/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_dock_target_model.c
 *
 * PURPOSE:
 *   Exercise dock target model behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/dock_target_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicDockTargetModel value;
    umi_ui_mosaic_dock_target_model_init(&value);
    CHECK(umi_ui_mosaic_dock_target_model_set(&value, "panel.source", "panel.target", UMI_UI_MOSAIC_DOCK_CENTRE) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_target_model_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_target_model_is_centre(&value) == 1);
    return 0;
}
