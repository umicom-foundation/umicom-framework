/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/policy.c
 *
 * PURPOSE:
 *   Express release policy requirements and evaluate build evidence against them.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Policy keeps channel rules out of product-specific scripts and makes release decisions reproducible.
 */

#include "umicom/delivery/policy.h"

/*
 * Provide the delivery policy default operation used by this module and its client
 * applications.
 */
UmiDeliveryPolicy umi_delivery_policy_default(void)
{
    UmiDeliveryPolicy policy = {1, 0, 0, 0, 1};
    return policy;
}
/*
 * Provide the delivery policy check build operation used by this module and its client
 * applications.
 */
UmiStatus umi_delivery_policy_check_build(const UmiDeliveryPolicy *policy,
                                          const UmiBuildEvidence *evidence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Create this optional product surface only when its build option is enabled. */
    if (!evidence->build_succeeded) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_tests && !umi_build_evidence_passed(evidence)) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_zero_warnings && evidence->warnings != 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
