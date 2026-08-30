/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_display_metrics.c
 *
 * PURPOSE:
 *   Validate convert physical pixel metrics and scale factors into stable logical viewport dimensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/display_metrics.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveDisplayMetrics metrics;
    UmiAdaptiveViewport viewport;
    CHECK(umi_adaptive_display_metrics_init(&metrics, 2880, 1800, 220U, 2.0) == UMI_STATUS_OK);
    CHECK(umi_adaptive_display_metrics_logical_viewport(&metrics, &viewport) == UMI_STATUS_OK);
    CHECK(viewport.width == 1440 && viewport.height == 900);
    return 0;
}
