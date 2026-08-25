#include <stdio.h>
#include "umicom/observability/performance/threshold_policy.h"

int main(void) {
    UmiPerformanceThresholdPolicy left;
    UmiPerformanceThresholdPolicy right;
    if (umi_performance_threshold_policy_init(&left, "threshold_policy", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_threshold_policy_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_threshold_policy_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_threshold_policy_transition_allowed(UMI_PERFORMANCE_STATE_IDLE, UMI_PERFORMANCE_STATE_ACTIVE)) return 4;
    if (umi_performance_threshold_policy_init(&right, "threshold_policy", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_threshold_policy_same_identity(&left, &right)) return 6;
    puts("threshold_policy: ok");
    return 0;
}
