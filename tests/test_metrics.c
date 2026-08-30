/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_metrics.c
 *
 * PURPOSE:
 *   Verify counter and gauge metric registration and updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/diagnostics/metrics.h"
int main(void) {
    UmiMetricsRegistry *r = NULL; UmiMetricSnapshot m;
    assert(umi_metrics_registry_create(&r) == UMI_STATUS_OK);
    assert(umi_metrics_registry_define(r, "builds.total", "Build attempts", UMI_METRIC_COUNTER) == UMI_STATUS_OK);
    assert(umi_metrics_counter_add(r, "builds.total", 2.0, 10U) == UMI_STATUS_OK);
    assert(umi_metrics_registry_get(r, "builds.total", &m) == UMI_STATUS_OK && m.value == 2.0);
    assert(umi_metrics_registry_define(r, "queue.depth", "Queue depth", UMI_METRIC_GAUGE) == UMI_STATUS_OK);
    assert(umi_metrics_gauge_set(r, "queue.depth", 4.0, 11U) == UMI_STATUS_OK);
    umi_metrics_registry_destroy(r); return 0;
}
