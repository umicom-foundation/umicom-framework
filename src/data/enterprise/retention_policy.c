/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/retention_policy.c
 *
 * PURPOSE:
 *   Decide whether backup evidence is eligible for retention or deletion by age and minimum retained copies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/retention_policy.h"
#include <string.h>

/* Defaults favour bounded, reviewable behaviour over aggressive execution. */
void umi_data_retention_policy_default(UmiDataRetentionPolicy *policy) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->max_age_ms = 1000U;
    policy->minimum_copies = 3U;
    policy->preserve_last_success = true;
}

/* Evaluation is side-effect free so callers can preview decisions. */
UmiStatus umi_data_retention_policy_evaluate(const UmiDataRetentionPolicy *policy, uint64_t age_ms, size_t successful_copies, bool is_last_success, bool *out_allowed) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || out_allowed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_allowed = age_ms > policy->max_age_ms && successful_copies > policy->minimum_copies && (!policy->preserve_last_success || !is_last_success);
    return UMI_STATUS_OK;
}
