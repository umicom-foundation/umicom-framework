/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/model_fallback.c
 *
 * PURPOSE:
 *   Decide whether a failed request may fail over to another model.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/model_fallback.h"

#include <string.h>

void umi_ai_dev_model_fallback_init(UmiAiDevModelFallback *policy) {
    if (policy == NULL) {
        return;
    }
    memset(policy, 0, sizeof(*policy));
    policy->maximum_risk = UMI_AI_DEV_RISK_LOW;
    policy->locality = UMI_AI_DEV_LOCALITY_ANY;
    policy->enabled = 1;
}
UmiStatus umi_ai_dev_model_fallback_configure(UmiAiDevModelFallback *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval) {
    if (policy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_ai_dev_model_fallback_init(policy);
    policy->allowed_flags = allowed_flags; policy->denied_flags = denied_flags; policy->maximum_risk = maximum_risk; policy->locality = locality; policy->require_approval = require_approval != 0; return UMI_STATUS_OK;
}
int umi_ai_dev_model_fallback_allows(const UmiAiDevModelFallback *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality) {
    if (policy == NULL || !policy->enabled || risk > policy->maximum_risk) return 0;
    if ((requested_flags & policy->denied_flags) != 0U) return 0;
    if (policy->allowed_flags != 0U && (requested_flags & policy->allowed_flags) != requested_flags) return 0;
    if (policy->locality != UMI_AI_DEV_LOCALITY_ANY && locality != policy->locality) return 0;
    return 1;
}
int umi_ai_dev_model_fallback_approval_required(const UmiAiDevModelFallback *policy, UmiAiDevRisk risk) { return policy != NULL && policy->require_approval && risk >= UMI_AI_DEV_RISK_MEDIUM; }
