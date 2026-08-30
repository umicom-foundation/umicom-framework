/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/validation_plan.c
 * PURPOSE: Enforce configure, build, focused-test and complete-test ordering.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/validation_plan.h"

#include <stdio.h>
#include <string.h>

static UmiStatus set_step(UmiTestPlatformValidationStep *step,
                          UmiTestPlatformValidationStepKind kind,
                          const char *format, const char *preset,
                          const char *test_regex)
{
    int written;
    step->kind = kind;
    written = snprintf(step->command, sizeof(step->command), format, preset,
                       test_regex);
    if (written < 0 || (size_t)written >= sizeof(step->command))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_validation_plan_create(
    const UmiTestPlatformProductValidationProfile *profile,
    UmiTestPlatformValidationPlan *plan)
{
    UmiStatus status;
    if (plan == NULL ||
        umi_test_platform_product_validation_profile_validate(profile) !=
            UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(plan, 0, sizeof(*plan));
    status = set_step(&plan->steps[0],
        UMI_TEST_PLATFORM_VALIDATION_STEP_CONFIGURE,
        "cmake --fresh --preset %s", profile->preset, "");
    if (status != UMI_STATUS_OK) return status;
    status = set_step(&plan->steps[1],
        UMI_TEST_PLATFORM_VALIDATION_STEP_BUILD_ARTIFACTS,
        "cmake --build --preset %s --target umicom-registered-validation-tests --parallel 2",
        profile->preset, "");
    if (status != UMI_STATUS_OK) return status;
    status = set_step(&plan->steps[2],
        UMI_TEST_PLATFORM_VALIDATION_STEP_RUN_FOCUSED,
        "ctest --preset %s -R \"%s\" --output-on-failure",
        profile->preset, profile->test_regex);
    if (status != UMI_STATUS_OK) return status;
    status = set_step(&plan->steps[3],
        UMI_TEST_PLATFORM_VALIDATION_STEP_RUN_COMPLETE,
        "ctest --preset %s --output-on-failure", profile->preset, "");
    if (status != UMI_STATUS_OK) return status;
    plan->count = UMI_TEST_PLATFORM_VALIDATION_PLAN_CAPACITY;
    return UMI_STATUS_OK;
}

const UmiTestPlatformValidationStep *umi_test_platform_validation_plan_step(
    const UmiTestPlatformValidationPlan *plan, size_t index)
{
    return plan != NULL && index < plan->count ? &plan->steps[index] : NULL;
}
