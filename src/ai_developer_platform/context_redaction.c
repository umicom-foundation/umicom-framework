/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/context_redaction.c
 *
 * PURPOSE:
 *   Decide whether sensitive context must be excluded or redacted before provider use.
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
#include "umicom/ai/developer_platform/context_redaction.h"

#include <string.h>

/*
 * Initialise ai dev context redaction from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_context_redaction_init(UmiAiDevContextRedaction *policy) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) {
        return;
    }
    memset(policy, 0, sizeof(*policy));
    policy->maximum_risk = UMI_AI_DEV_RISK_LOW;
    policy->locality = UMI_AI_DEV_LOCALITY_ANY;
    policy->enabled = 1;
}
/*
 * Provide the ai dev context redaction configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_context_redaction_configure(UmiAiDevContextRedaction *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_ai_dev_context_redaction_init(policy);
    policy->allowed_flags = allowed_flags; policy->denied_flags = denied_flags; policy->maximum_risk = maximum_risk; policy->locality = locality; policy->require_approval = require_approval != 0; return UMI_STATUS_OK;
}
/*
 * Provide the ai dev context redaction allows operation used by this module and its client
 * applications.
 */
int umi_ai_dev_context_redaction_allows(const UmiAiDevContextRedaction *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || !policy->enabled || risk > policy->maximum_risk) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((requested_flags & policy->denied_flags) != 0U) return 0;
    /* Apply this operation only while the related capability or state is available. */
    if (policy->allowed_flags != 0U && (requested_flags & policy->allowed_flags) != requested_flags) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->locality != UMI_AI_DEV_LOCALITY_ANY && locality != policy->locality) return 0;
    return 1;
}
/*
 * Provide the ai dev context redaction approval required operation used by this module and
 * its client applications.
 */
int umi_ai_dev_context_redaction_approval_required(const UmiAiDevContextRedaction *policy, UmiAiDevRisk risk) { return policy != NULL && policy->require_approval && risk >= UMI_AI_DEV_RISK_MEDIUM; }
