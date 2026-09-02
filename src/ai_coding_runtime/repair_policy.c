/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/repair_policy.c
 *
 * PURPOSE:
 *   Implement deterministic validation repair decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/repair_policy.h"

/*
 * Initialise ai coding repair policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_repair_policy_init(UmiAiCodingRepairPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;

    policy->maximum_iterations = 3U;
    policy->rollback_before_retry = 1;
    policy->retry_optional_failures = 0;
    policy->retry_required_failures = 1;
}

/*
 * Provide the ai coding repair decide operation used by this module and its client
 * applications.
 */
UmiAiCodingRepairDecision umi_ai_coding_repair_decide(
    const UmiAiCodingRepairPolicy *policy,
    const UmiAiCodingValidationReport *report,
    uint32_t current_iteration)
{
    int retry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || report == NULL) {
        return UMI_AI_CODING_REPAIR_FAIL;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (report->passed) return UMI_AI_CODING_REPAIR_ACCEPT;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (current_iteration >= policy->maximum_iterations) {
        return UMI_AI_CODING_REPAIR_FAIL;
    }

    retry =
        (report->required_failed_count > 0U &&
         policy->retry_required_failures) ||
        (report->required_failed_count == 0U &&
         report->failed_count > 0U &&
         policy->retry_optional_failures);

    /* Apply this branch only when its contract condition is satisfied. */
    if (!retry) return UMI_AI_CODING_REPAIR_FAIL;

    return policy->rollback_before_retry
        ? UMI_AI_CODING_REPAIR_ROLLBACK_AND_RETRY
        : UMI_AI_CODING_REPAIR_RETRY;
}
