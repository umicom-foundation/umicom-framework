/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_monitor_affinity.c
 *
 * PURPOSE:
 *   Exercise monitor affinity behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/monitor_affinity.h"
int main(void) {
    UmiUiMosaicMonitorAffinity value;
    umi_ui_mosaic_monitor_affinity_init(&value);
    CHECK(umi_ui_mosaic_monitor_affinity_set(&value, "layout.monitor_affinity", "Monitor Affinity") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_monitor_affinity_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_monitor_affinity_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
