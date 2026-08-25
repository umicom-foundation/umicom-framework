#include <stdio.h>
#include "umicom/observability/performance/metric_series.h"

int main(void) {
    UmiPerformanceMetricSeries left;
    UmiPerformanceMetricSeries right;
    if (umi_performance_metric_series_init(&left, "metric_series", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_metric_series_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_metric_series_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_metric_series_capacity_available(1U, 2U)) return 4;
    if (umi_performance_metric_series_init(&right, "metric_series", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_metric_series_same_identity(&left, &right)) return 6;
    puts("metric_series: ok");
    return 0;
}
