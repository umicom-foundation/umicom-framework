/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_designer_inspector.c
 *
 * PURPOSE:
 *   Exercise designer inspector behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/designer_inspector.h"
int main(void) {
    UmiUiMosaicDesignerInspector value;
    umi_ui_mosaic_designer_inspector_init(&value);
    CHECK(umi_ui_mosaic_designer_inspector_bind(&value, "workspace.main", "panel.active") == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_designer_inspector_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_designer_inspector_advance(&value) == UMI_STATUS_PERMISSION_DENIED);
    value.mode = UMI_UI_MOSAIC_EDIT_UNLOCKED;
    CHECK(umi_ui_mosaic_designer_inspector_advance(&value) == UMI_STATUS_OK);
    return 0;
}
