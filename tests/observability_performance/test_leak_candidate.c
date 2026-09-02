/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_leak_candidate.c
 *
 * PURPOSE:
 *   Implement the test leak candidate behavior for
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
#include "umicom/observability/performance/leak_candidate.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceLeakCandidate left;
    UmiPerformanceLeakCandidate right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_leak_candidate_init(&left, "leak_candidate", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_leak_candidate_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_leak_candidate_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_leak_candidate_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_leak_candidate_init(&right, "leak_candidate", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_leak_candidate_same_identity(&left, &right)) return 6;
    puts("leak_candidate: ok");
    return 0;
}
