/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_designer_drop_preview.c
 *
 * PURPOSE:
 *   Exercise designer drop preview behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/designer_drop_preview.h"
int main(void) {
    UmiUiMosaicDesignerDropPreview value;
    umi_ui_mosaic_designer_drop_preview_init(&value);
    CHECK(umi_ui_mosaic_designer_drop_preview_bind(&value, "workspace.main", "panel.active") == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_designer_drop_preview_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_designer_drop_preview_advance(&value) == UMI_STATUS_PERMISSION_DENIED);
    value.mode = UMI_UI_MOSAIC_EDIT_UNLOCKED;
    CHECK(umi_ui_mosaic_designer_drop_preview_advance(&value) == UMI_STATUS_OK);
    return 0;
}
