/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_workspace_descriptor.c
 *
 * PURPOSE:
 *   Exercise workspace descriptor behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/workspace_descriptor.h"
int main(void) {
    UmiUiMosaicWorkspaceDescriptor value;
    umi_ui_mosaic_workspace_descriptor_init(&value);
    CHECK(umi_ui_mosaic_workspace_descriptor_set(&value, "perspective.workspace_descriptor", "Workspace Descriptor", "layout.default", UMI_UI_MOSAIC_APP_TMS) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_workspace_descriptor_validate(&value) == UMI_STATUS_OK);
    return 0;
}
