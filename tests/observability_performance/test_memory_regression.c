#include <stdio.h>
#include "umicom/observability/performance/memory_regression.h"

int main(void) {
    UmiPerformanceMemoryRegression left;
    UmiPerformanceMemoryRegression right;
    if (umi_performance_memory_regression_init(&left, "memory_regression", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_memory_regression_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_memory_regression_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_memory_regression_regression_percent(120.0, 100.0) != 20.0) return 4;
    if (umi_performance_memory_regression_init(&right, "memory_regression", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_memory_regression_same_identity(&left, &right)) return 6;
    puts("memory_regression: ok");
    return 0;
}
