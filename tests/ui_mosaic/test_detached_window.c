/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_detached_window.c
 *
 * PURPOSE:
 *   Exercise detached window behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/detached_window.h"
int main(void) {
    UmiUiMosaicDetachedWindow value;
    umi_ui_mosaic_detached_window_init(&value);
    CHECK(umi_ui_mosaic_detached_window_set(&value, "layout.detached_window", "Detached Window") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_detached_window_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_detached_window_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
