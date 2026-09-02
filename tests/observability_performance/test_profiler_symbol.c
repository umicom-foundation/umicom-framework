/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_profiler_symbol.c
 *
 * PURPOSE:
 *   Implement the test profiler symbol behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/observability/performance/profiler_symbol.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceProfilerSymbol left;
    UmiPerformanceProfilerSymbol right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_profiler_symbol_init(&left, "profiler_symbol", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_profiler_symbol_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_profiler_symbol_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_profiler_symbol_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_profiler_symbol_init(&right, "profiler_symbol", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_profiler_symbol_same_identity(&left, &right)) return 6;
    puts("profiler_symbol: ok");
    return 0;
}
