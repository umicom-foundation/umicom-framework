/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/types.h
 * PURPOSE: Define portable states for the pre-CTest build-readiness pipeline.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_TYPES_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_BUILD_READINESS_API_VERSION 1U

/**
 * List the named test platform build readiness state values accepted by this public
 * contract.
 */
typedef enum UmiTestPlatformBuildReadinessState {
    UMI_TEST_PLATFORM_BUILD_STATE_UNKNOWN = 0,
    UMI_TEST_PLATFORM_BUILD_STATE_READY = 1,
    UMI_TEST_PLATFORM_BUILD_STATE_CONFIGURE_FAILED = 2,
    UMI_TEST_PLATFORM_BUILD_STATE_BUILD_FAILED = 3,
    UMI_TEST_PLATFORM_BUILD_STATE_TARGET_DISABLED = 4,
    UMI_TEST_PLATFORM_BUILD_STATE_TEST_UNREGISTERED = 5,
    UMI_TEST_PLATFORM_BUILD_STATE_ARTIFACT_MISSING = 6
} UmiTestPlatformBuildReadinessState;

/**
 * List the named test platform validation step kind values accepted by this public
 * contract.
 */
typedef enum UmiTestPlatformValidationStepKind {
    UMI_TEST_PLATFORM_VALIDATION_STEP_CONFIGURE = 0,
    UMI_TEST_PLATFORM_VALIDATION_STEP_BUILD_ARTIFACTS = 1,
    UMI_TEST_PLATFORM_VALIDATION_STEP_RUN_FOCUSED = 2,
    UMI_TEST_PLATFORM_VALIDATION_STEP_RUN_COMPLETE = 3
} UmiTestPlatformValidationStepKind;

/**
 * Provide the test platform build readiness state text operation used by this module and
 * its client applications.
 */
const char *umi_test_platform_build_readiness_state_text(
    UmiTestPlatformBuildReadinessState state);
/**
 * Provide the test platform validation step kind text operation used by this module and
 * its client applications.
 */
const char *umi_test_platform_validation_step_kind_text(
    UmiTestPlatformValidationStepKind kind);

#ifdef __cplusplus
}
#endif
#endif
