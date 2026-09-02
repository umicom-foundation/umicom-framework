/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/failover_policy.c
 *
 * PURPOSE:
 *   Gate replica promotion based on health, lag and explicit promotion permission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/failover_policy.h"
#include <string.h>

/* Defaults favour bounded, reviewable behaviour over aggressive execution. */
void umi_data_failover_policy_default(UmiDataFailoverPolicy *policy) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->maximum_promotion_lag_ms = 1000U;
    policy->automatic_failover = true;
    policy->require_manual_approval = true;
}

/* Evaluation is side-effect free so callers can preview decisions. */
UmiStatus umi_data_failover_policy_evaluate(const UmiDataFailoverPolicy *policy, bool candidate_healthy, uint64_t lag_ms, bool approved, bool *out_allowed) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || out_allowed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_allowed = candidate_healthy && lag_ms <= policy->maximum_promotion_lag_ms && policy->automatic_failover && (!policy->require_manual_approval || approved);
    return UMI_STATUS_OK;
}
