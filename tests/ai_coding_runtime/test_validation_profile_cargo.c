/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_validation_profile_cargo.c
 *
 * PURPOSE:
 *   Verify the reusable cargo AI coding validation profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/ai_coding_runtime/validation_profiles/cargo.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiCodingValidationPlan plan;
    assert(umi_ai_coding_validation_profile_cargo(
        "C:/workspace", &plan) == UMI_STATUS_OK);
    assert(plan.step_count == 2U);
    assert(plan.steps[0].executable[0] != '\0');
    return 0;
}
