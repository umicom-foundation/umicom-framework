#include <stdio.h>
#include "umicom/observability/performance/crash_correlation.h"

int main(void) {
    UmiPerformanceCrashCorrelation left;
    UmiPerformanceCrashCorrelation right;
    if (umi_performance_crash_correlation_init(&left, "crash_correlation", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_crash_correlation_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_crash_correlation_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_crash_correlation_ratio(10.0, 2.0) != 5.0) return 4;
    if (umi_performance_crash_correlation_init(&right, "crash_correlation", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_crash_correlation_same_identity(&left, &right)) return 6;
    puts("crash_correlation: ok");
    return 0;
}
