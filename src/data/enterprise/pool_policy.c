/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/pool_policy.c
 *
 * PURPOSE:
 *   Gate pool expansion, leasing and retirement using explicit capacity thresholds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/pool_policy.h"
#include <string.h>

/* Defaults favour bounded, reviewable behaviour over aggressive execution. */
void umi_data_pool_policy_default(UmiDataPoolPolicy *policy) {
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->minimum_size = 3U;
    policy->maximum_size = 3U;
    policy->high_utilisation_percent = 80.0;
}

/* Evaluation is side-effect free so callers can preview decisions. */
UmiStatus umi_data_pool_policy_evaluate(const UmiDataPoolPolicy *policy, size_t current_size, double utilisation_percent, bool *out_allowed) {
    if (policy == NULL || out_allowed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_allowed = current_size < policy->maximum_size && utilisation_percent >= policy->high_utilisation_percent;
    return UMI_STATUS_OK;
}
