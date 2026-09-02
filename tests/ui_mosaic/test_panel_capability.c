/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_capability.c
 *
 * PURPOSE:
 *   Exercise panel capability behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_capability.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicPanelCapability value;
    umi_ui_mosaic_panel_capability_init(&value);
    CHECK(umi_ui_mosaic_panel_capability_set(&value, "panel.panel_capability", "Panel Capability") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_capability_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_capability_rank(&value, 5U) == 16U);
    return 0;
}
