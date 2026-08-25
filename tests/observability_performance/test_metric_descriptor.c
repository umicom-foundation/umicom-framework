#include <stdio.h>
#include "umicom/observability/performance/metric_descriptor.h"

int main(void) {
    UmiPerformanceMetricDescriptor left;
    UmiPerformanceMetricDescriptor right;
    if (umi_performance_metric_descriptor_init(&left, "metric_descriptor", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_metric_descriptor_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_metric_descriptor_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_metric_descriptor_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_metric_descriptor_init(&right, "metric_descriptor", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_metric_descriptor_same_identity(&left, &right)) return 6;
    puts("metric_descriptor: ok");
    return 0;
}
