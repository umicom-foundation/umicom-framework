/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/validation_plan.h
 * PURPOSE: Generate configure, artifact-build and CTest commands in safe order.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_VALIDATION_PLAN_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_VALIDATION_PLAN_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/build_readiness/product_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_VALIDATION_PLAN_CAPACITY 4U

typedef struct UmiTestPlatformValidationStep {
    UmiTestPlatformValidationStepKind kind;
    char command[512];
} UmiTestPlatformValidationStep;

typedef struct UmiTestPlatformValidationPlan {
    UmiTestPlatformValidationStep
        steps[UMI_TEST_PLATFORM_VALIDATION_PLAN_CAPACITY];
    size_t count;
} UmiTestPlatformValidationPlan;

UmiStatus umi_test_platform_validation_plan_create(
    const UmiTestPlatformProductValidationProfile *profile,
    UmiTestPlatformValidationPlan *plan);
const UmiTestPlatformValidationStep *umi_test_platform_validation_plan_step(
    const UmiTestPlatformValidationPlan *plan,
    size_t index);

#ifdef __cplusplus
}
#endif
#endif
