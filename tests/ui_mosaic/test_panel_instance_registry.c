/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_instance_registry.c
 *
 * PURPOSE:
 *   Exercise panel instance registry behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_instance_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicPanelInstanceRegistry value;
    umi_ui_mosaic_panel_instance_registry_init(&value);
    CHECK(umi_ui_mosaic_panel_instance_registry_set(&value, "panel.panel_instance_registry", "Panel Instance Registry") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_instance_registry_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_instance_registry_rank(&value, 5U) == 16U);
    return 0;
}
