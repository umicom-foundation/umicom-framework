/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_validation_profile_make.c
 *
 * PURPOSE:
 *   Verify the reusable make AI coding validation profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/ai_coding_runtime/validation_profiles/make.h"

int main(void)
{
    UmiAiCodingValidationPlan plan;
    assert(umi_ai_coding_validation_profile_make(
        "C:/workspace", &plan) == UMI_STATUS_OK);
    assert(plan.step_count == 2U);
    assert(plan.steps[0].executable[0] != '\0');
    return 0;
}
