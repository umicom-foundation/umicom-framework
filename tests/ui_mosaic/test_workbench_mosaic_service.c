/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_workbench_mosaic_service.c
 *
 * PURPOSE:
 *   Exercise workbench mosaic service behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/workbench_mosaic_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicWorkbenchMosaicService value;
    umi_ui_mosaic_workbench_mosaic_service_init(&value);
    CHECK(umi_ui_mosaic_workbench_mosaic_service_bind(&value, "workspace.main", "panel.active") == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_workbench_mosaic_service_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_workbench_mosaic_service_advance(&value) == UMI_STATUS_PERMISSION_DENIED);
    value.mode = UMI_UI_MOSAIC_EDIT_UNLOCKED;
    CHECK(umi_ui_mosaic_workbench_mosaic_service_advance(&value) == UMI_STATUS_OK);
    return 0;
}
