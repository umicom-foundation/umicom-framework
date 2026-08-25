/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/consistency_policy.c
 *
 * PURPOSE:
 *   Decide whether observed replica lag is acceptable for a requested consistency model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/consistency_policy.h"
#include <string.h>

/* Defaults favour bounded, reviewable behaviour over aggressive execution. */
void umi_data_consistency_policy_default(UmiDataConsistencyPolicy *policy) {
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->minimum_consistency = UMI_DATA_CONSISTENCY_SESSION;
    policy->session_lag_ms = 1000U;
    policy->eventual_lag_ms = 1000U;
}

/* Evaluation is side-effect free so callers can preview decisions. */
UmiStatus umi_data_consistency_policy_evaluate(const UmiDataConsistencyPolicy *policy, UmiDataConsistency requested, uint64_t observed_lag_ms, bool is_primary, bool *out_allowed) {
    if (policy == NULL || out_allowed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (requested == UMI_DATA_CONSISTENCY_STRONG) *out_allowed = is_primary; else if (requested == UMI_DATA_CONSISTENCY_SESSION) *out_allowed = is_primary || observed_lag_ms <= policy->session_lag_ms; else *out_allowed = is_primary || observed_lag_ms <= policy->eventual_lag_ms;
    return UMI_STATUS_OK;
}
