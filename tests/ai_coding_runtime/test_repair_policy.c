/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_repair_policy.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime repair policy contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/repair_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    UmiAiCodingRepairPolicy policy;
    UmiAiCodingValidationReport report = {0};
    umi_ai_coding_repair_policy_init(&policy);
    report.passed = 0;
    report.failed_count = 1U;
    report.required_failed_count = 1U;
    assert(umi_ai_coding_repair_decide(
        &policy, &report, 1U) == UMI_AI_CODING_REPAIR_ROLLBACK_AND_RETRY);
    report.passed = 1;
    assert(umi_ai_coding_repair_decide(
        &policy, &report, 1U) == UMI_AI_CODING_REPAIR_ACCEPT);

    return 0;
}
