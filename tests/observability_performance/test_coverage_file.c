#include <stdio.h>
#include "umicom/observability/performance/coverage_file.h"

int main(void) {
    UmiPerformanceCoverageFile left;
    UmiPerformanceCoverageFile right;
    if (umi_performance_coverage_file_init(&left, "coverage_file", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_coverage_file_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_coverage_file_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_coverage_file_coverage_percent(80U, 100U) != 80.0) return 4;
    if (umi_performance_coverage_file_init(&right, "coverage_file", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_coverage_file_same_identity(&left, &right)) return 6;
    puts("coverage_file: ok");
    return 0;
}
