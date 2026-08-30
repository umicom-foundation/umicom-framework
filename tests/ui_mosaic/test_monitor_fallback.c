/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_monitor_fallback.c
 *
 * PURPOSE:
 *   Exercise monitor fallback behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/monitor_fallback.h"
int main(void) {
    UmiUiMosaicMonitorFallback value;
    umi_ui_mosaic_monitor_fallback_init(&value);
    CHECK(umi_ui_mosaic_monitor_fallback_set(&value, "layout.monitor_fallback", "Monitor Fallback") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_monitor_fallback_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_monitor_fallback_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
