/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_dock_transaction.c
 *
 * PURPOSE:
 *   Exercise dock transaction behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/dock_transaction.h"
int main(void) {
    UmiUiMosaicDockTransaction value;
    umi_ui_mosaic_dock_transaction_init(&value);
    CHECK(umi_ui_mosaic_dock_transaction_set(&value, "panel.source", "panel.target", UMI_UI_MOSAIC_DOCK_CENTRE) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_transaction_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_transaction_is_centre(&value) == 1);
    return 0;
}
