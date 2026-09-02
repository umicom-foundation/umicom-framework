/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_threshold_policy.c
 *
 * PURPOSE:
 *   Implement the test threshold policy behavior for
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
#include "umicom/observability/performance/threshold_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceThresholdPolicy left;
    UmiPerformanceThresholdPolicy right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_threshold_policy_init(&left, "threshold_policy", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_threshold_policy_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_threshold_policy_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_threshold_policy_transition_allowed(UMI_PERFORMANCE_STATE_IDLE, UMI_PERFORMANCE_STATE_ACTIVE)) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_threshold_policy_init(&right, "threshold_policy", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_threshold_policy_same_identity(&left, &right)) return 6;
    puts("threshold_policy: ok");
    return 0;
}
