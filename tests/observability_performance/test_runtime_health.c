#include <stdio.h>
#include "umicom/observability/performance/runtime_health.h"

int main(void) {
    UmiPerformanceRuntimeHealth left;
    UmiPerformanceRuntimeHealth right;
    if (umi_performance_runtime_health_init(&left, "runtime_health", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_runtime_health_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_runtime_health_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_runtime_health_healthy(UMI_PERFORMANCE_SEVERITY_WARNING, false)) return 4;
    if (umi_performance_runtime_health_init(&right, "runtime_health", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_runtime_health_same_identity(&left, &right)) return 6;
    puts("runtime_health: ok");
    return 0;
}
