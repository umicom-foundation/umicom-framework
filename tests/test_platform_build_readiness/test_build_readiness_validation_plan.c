/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_validation_plan.c
 * PURPOSE: Focused regression for the Framework build-readiness platform.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/build_readiness/validation_plan.h"
int main(void) {
    UmiTestPlatformProductValidationProfile profile;
    UmiTestPlatformValidationPlan plan;
    assert(umi_test_platform_product_validation_profile_init(&profile, "studio",
        "Umicom Studio IDE", "windows-ucrt64-debug", "studio", true, false) ==
        UMI_STATUS_OK);
    assert(umi_test_platform_validation_plan_create(&profile, &plan) ==
        UMI_STATUS_OK);
    assert(plan.count == 4U);
    assert(strstr(plan.steps[0].command, "cmake --fresh") != NULL);
    assert(strstr(plan.steps[1].command,
        "umicom-registered-validation-tests") != NULL);
    assert(strstr(plan.steps[2].command, "-R \"studio\"") != NULL);
    assert(umi_test_platform_validation_plan_step(&plan, 4U) == NULL);
    return 0;
}

