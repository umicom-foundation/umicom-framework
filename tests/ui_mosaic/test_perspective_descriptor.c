/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_perspective_descriptor.c
 *
 * PURPOSE:
 *   Exercise perspective descriptor behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/perspective_descriptor.h"
int main(void) {
    UmiUiMosaicPerspectiveDescriptor value;
    umi_ui_mosaic_perspective_descriptor_init(&value);
    CHECK(umi_ui_mosaic_perspective_descriptor_set(&value, "perspective.perspective_descriptor", "Perspective Descriptor", "layout.default", UMI_UI_MOSAIC_APP_TMS) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_perspective_descriptor_validate(&value) == UMI_STATUS_OK);
    return 0;
}
