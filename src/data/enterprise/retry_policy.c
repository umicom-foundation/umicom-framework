/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/retry_policy.c
 *
 * PURPOSE:
 *   Bound retry attempts for transient data operations without retrying unsafe work indefinitely.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/retry_policy.h"
#include <string.h>

/* Defaults favour bounded, reviewable behaviour over aggressive execution. */
void umi_data_retry_policy_default(UmiDataRetryPolicy *policy) {
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->max_attempts = 3U;
    policy->base_delay_ms = 1000U;
    policy->max_delay_ms = 1000U;
}

/* Evaluation is side-effect free so callers can preview decisions. */
UmiStatus umi_data_retry_policy_evaluate(const UmiDataRetryPolicy *policy, uint32_t attempt, bool transient_failure, bool *out_allowed) {
    if (policy == NULL || out_allowed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_allowed = transient_failure && attempt < policy->max_attempts;
    return UMI_STATUS_OK;
}
