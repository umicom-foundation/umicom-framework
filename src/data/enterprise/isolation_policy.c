/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/isolation_policy.c
 *
 * PURPOSE:
 *   Gate operations by requested transaction isolation strength.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/isolation_policy.h"
#include <string.h>

/* Defaults favour bounded, reviewable behaviour over aggressive execution. */
void umi_data_isolation_policy_default(UmiDataIsolationPolicy *policy) {
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->minimum_level = 3U;
    policy->maximum_level = 3U;
    policy->allow_upgrade = true;
}

/* Evaluation is side-effect free so callers can preview decisions. */
UmiStatus umi_data_isolation_policy_evaluate(const UmiDataIsolationPolicy *policy, uint32_t requested_level, bool *out_allowed) {
    if (policy == NULL || out_allowed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_allowed = requested_level >= policy->minimum_level && requested_level <= policy->maximum_level;
    return UMI_STATUS_OK;
}
