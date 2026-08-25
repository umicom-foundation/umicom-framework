#include <stdio.h>
#include "umicom/observability/performance/histogram.h"

int main(void) {
    UmiPerformanceHistogram left;
    UmiPerformanceHistogram right;
    if (umi_performance_histogram_init(&left, "histogram", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_histogram_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_histogram_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_histogram_exceeds(11.0, 10.0)) return 4;
    if (umi_performance_histogram_init(&right, "histogram", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_histogram_same_identity(&left, &right)) return 6;
    puts("histogram: ok");
    return 0;
}
