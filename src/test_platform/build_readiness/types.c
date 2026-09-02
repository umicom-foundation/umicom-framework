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

/*
 * Provide the test platform build readiness state text operation used by this module and
 * its client applications.
 */
const char *umi_test_platform_build_readiness_state_text(
    UmiTestPlatformBuildReadinessState state)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the test platform validation step kind text operation used by this module and
 * its client applications.
 */
const char *umi_test_platform_validation_step_kind_text(
    UmiTestPlatformValidationStepKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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
