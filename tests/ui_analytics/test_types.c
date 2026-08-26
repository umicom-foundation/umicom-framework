/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_types.c
 *
 * PURPOSE:
 *   Validate types analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/types.h"

int main(void) {
    char text[8];
    if (umi_analytics_copy_text(text, sizeof text, "metric") != UMI_STATUS_OK) return 1;
    if (!umi_analytics_number_valid(12.5)) return 2;
    if (!umi_analytics_compare(5.0, UMI_ANALYTICS_COMPARE_GT, 4.0)) return 3;
    if (umi_analytics_hash_text("metric") == 0U) return 4;
    return 0;
}
