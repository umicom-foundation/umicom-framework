/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_dock_history.c
 *
 * PURPOSE:
 *   Exercise dock history behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/dock_history.h"
int main(void) {
    UmiUiMosaicDockHistory value;
    umi_ui_mosaic_dock_history_init(&value);
    CHECK(umi_ui_mosaic_dock_history_set(&value, "panel.source", "panel.target", UMI_UI_MOSAIC_DOCK_CENTRE) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_history_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_history_is_centre(&value) == 1);
    return 0;
}
