/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/repair_policy.c
 *
 * PURPOSE:
 *   Implement deterministic validation repair decisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/repair_policy.h"

void umi_ai_coding_repair_policy_init(UmiAiCodingRepairPolicy *policy)
{
    if (policy == NULL) return;

    policy->maximum_iterations = 3U;
    policy->rollback_before_retry = 1;
    policy->retry_optional_failures = 0;
    policy->retry_required_failures = 1;
}

UmiAiCodingRepairDecision umi_ai_coding_repair_decide(
    const UmiAiCodingRepairPolicy *policy,
    const UmiAiCodingValidationReport *report,
    uint32_t current_iteration)
{
    int retry;

    if (policy == NULL || report == NULL) {
        return UMI_AI_CODING_REPAIR_FAIL;
    }

    if (report->passed) return UMI_AI_CODING_REPAIR_ACCEPT;

    if (current_iteration >= policy->maximum_iterations) {
        return UMI_AI_CODING_REPAIR_FAIL;
    }

    retry =
        (report->required_failed_count > 0U &&
         policy->retry_required_failures) ||
        (report->required_failed_count == 0U &&
         report->failed_count > 0U &&
         policy->retry_optional_failures);

    if (!retry) return UMI_AI_CODING_REPAIR_FAIL;

    return policy->rollback_before_retry
        ? UMI_AI_CODING_REPAIR_ROLLBACK_AND_RETRY
        : UMI_AI_CODING_REPAIR_RETRY;
}
