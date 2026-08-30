/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_validation_profile_meson.c
 *
 * PURPOSE:
 *   Verify the reusable meson AI coding validation profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/ai_coding_runtime/validation_profiles/meson.h"

int main(void)
{
    UmiAiCodingValidationPlan plan;
    assert(umi_ai_coding_validation_profile_meson(
        "C:/workspace", &plan) == UMI_STATUS_OK);
    assert(plan.step_count == 3U);
    assert(plan.steps[0].executable[0] != '\0');
    return 0;
}
