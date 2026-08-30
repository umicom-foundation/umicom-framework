/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_dock_direction.c
 *
 * PURPOSE:
 *   Exercise dock direction behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/dock_direction.h"
int main(void) {
    UmiUiMosaicDockDirection value;
    umi_ui_mosaic_dock_direction_init(&value);
    CHECK(umi_ui_mosaic_dock_direction_set(&value, "panel.source", "panel.target", UMI_UI_MOSAIC_DOCK_CENTRE) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_direction_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_direction_is_centre(&value) == 1);
    return 0;
}
