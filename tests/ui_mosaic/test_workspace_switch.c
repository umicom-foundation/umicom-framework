/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_workspace_switch.c
 *
 * PURPOSE:
 *   Exercise workspace switch behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/workspace_switch.h"
int main(void) {
    UmiUiMosaicWorkspaceSwitch value;
    umi_ui_mosaic_workspace_switch_init(&value);
    CHECK(umi_ui_mosaic_workspace_switch_set(&value, "perspective.workspace_switch", "Workspace Switch", "layout.default", UMI_UI_MOSAIC_APP_TMS) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_workspace_switch_validate(&value) == UMI_STATUS_OK);
    return 0;
}
