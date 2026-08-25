#include <stdio.h>
#include "umicom/observability/performance/coverage_baseline.h"

int main(void) {
    UmiPerformanceCoverageBaseline left;
    UmiPerformanceCoverageBaseline right;
    if (umi_performance_coverage_baseline_init(&left, "coverage_baseline", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_coverage_baseline_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_coverage_baseline_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_coverage_baseline_coverage_percent(80U, 100U) != 80.0) return 4;
    if (umi_performance_coverage_baseline_init(&right, "coverage_baseline", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_coverage_baseline_same_identity(&left, &right)) return 6;
    puts("coverage_baseline: ok");
    return 0;
}
