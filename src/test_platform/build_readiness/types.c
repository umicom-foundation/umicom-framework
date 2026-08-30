/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/types.c
 * PURPOSE: Render build-readiness states and validation steps for reports.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/types.h"

const char *umi_test_platform_build_readiness_state_text(
    UmiTestPlatformBuildReadinessState state)
{
    switch (state) {
        case UMI_TEST_PLATFORM_BUILD_STATE_UNKNOWN: return "unknown";
        case UMI_TEST_PLATFORM_BUILD_STATE_READY: return "ready";
        case UMI_TEST_PLATFORM_BUILD_STATE_CONFIGURE_FAILED:
            return "configure-failed";
        case UMI_TEST_PLATFORM_BUILD_STATE_BUILD_FAILED: return "build-failed";
        case UMI_TEST_PLATFORM_BUILD_STATE_TARGET_DISABLED:
            return "target-disabled";
        case UMI_TEST_PLATFORM_BUILD_STATE_TEST_UNREGISTERED:
            return "test-unregistered";
        case UMI_TEST_PLATFORM_BUILD_STATE_ARTIFACT_MISSING:
            return "artifact-missing";
        default: return "invalid";
    }
}

const char *umi_test_platform_validation_step_kind_text(
    UmiTestPlatformValidationStepKind kind)
{
    switch (kind) {
        case UMI_TEST_PLATFORM_VALIDATION_STEP_CONFIGURE: return "configure";
        case UMI_TEST_PLATFORM_VALIDATION_STEP_BUILD_ARTIFACTS:
            return "build-artifacts";
        case UMI_TEST_PLATFORM_VALIDATION_STEP_RUN_FOCUSED:
            return "run-focused";
        case UMI_TEST_PLATFORM_VALIDATION_STEP_RUN_COMPLETE:
            return "run-complete";
        default: return "invalid";
    }
}
