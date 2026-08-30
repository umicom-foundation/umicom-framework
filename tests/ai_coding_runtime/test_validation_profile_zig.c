/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_validation_profile_zig.c
 *
 * PURPOSE:
 *   Verify the reusable zig AI coding validation profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/ai_coding_runtime/validation_profiles/zig.h"

int main(void)
{
    UmiAiCodingValidationPlan plan;
    assert(umi_ai_coding_validation_profile_zig(
        "C:/workspace", &plan) == UMI_STATUS_OK);
    assert(plan.step_count == 2U);
    assert(plan.steps[0].executable[0] != '\0');
    return 0;
}
