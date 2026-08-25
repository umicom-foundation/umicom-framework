#include <stdio.h>
#include "umicom/observability/performance/resource_sample.h"

int main(void) {
    UmiPerformanceResourceSample left;
    UmiPerformanceResourceSample right;
    if (umi_performance_resource_sample_init(&left, "resource_sample", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_resource_sample_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_resource_sample_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_resource_sample_ratio(10.0, 2.0) != 5.0) return 4;
    if (umi_performance_resource_sample_init(&right, "resource_sample", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_resource_sample_same_identity(&left, &right)) return 6;
    puts("resource_sample: ok");
    return 0;
}
