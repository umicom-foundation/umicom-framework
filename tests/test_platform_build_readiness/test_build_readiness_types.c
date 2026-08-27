/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_types.c
 * PURPOSE: Focused regression for the Framework build-readiness platform.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/build_readiness/types.h"
int main(void) {
    assert(strcmp(umi_test_platform_build_readiness_state_text(
        UMI_TEST_PLATFORM_BUILD_STATE_ARTIFACT_MISSING), "artifact-missing") == 0);
    assert(strcmp(umi_test_platform_validation_step_kind_text(
        UMI_TEST_PLATFORM_VALIDATION_STEP_BUILD_ARTIFACTS), "build-artifacts") == 0);
    assert(strcmp(umi_test_platform_build_readiness_state_text(
        (UmiTestPlatformBuildReadinessState)99), "invalid") == 0);
    return 0;
}

