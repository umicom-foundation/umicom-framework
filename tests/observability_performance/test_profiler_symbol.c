#include <stdio.h>
#include "umicom/observability/performance/profiler_symbol.h"

int main(void) {
    UmiPerformanceProfilerSymbol left;
    UmiPerformanceProfilerSymbol right;
    if (umi_performance_profiler_symbol_init(&left, "profiler_symbol", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_symbol_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_symbol_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_profiler_symbol_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_profiler_symbol_init(&right, "profiler_symbol", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_symbol_same_identity(&left, &right)) return 6;
    puts("profiler_symbol: ok");
    return 0;
}
