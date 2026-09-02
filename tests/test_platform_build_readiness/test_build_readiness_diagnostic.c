/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_diagnostic.c
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
#include "umicom/test_platform/build_readiness/diagnostic.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTestPlatformBuildDiagnosticInput input = {0};
    UmiTestPlatformBuildDiagnostic diagnostic;
    assert(umi_test_platform_build_diagnostic_classify(&input, &diagnostic) ==
        UMI_STATUS_OK);
    assert(diagnostic.state == UMI_TEST_PLATFORM_BUILD_STATE_CONFIGURE_FAILED);
    input.configure_completed = true;
    input.target_enabled = true;
    input.build_completed = true;
    input.test_registered = true;
    assert(umi_test_platform_build_diagnostic_classify(&input, &diagnostic) ==
        UMI_STATUS_OK);
    assert(diagnostic.state == UMI_TEST_PLATFORM_BUILD_STATE_ARTIFACT_MISSING);
    assert(strcmp(diagnostic.remediation_command_id,
        "test.build.required-artifacts") == 0);
    input.executable_exists = true;
    assert(umi_test_platform_build_diagnostic_classify(&input, &diagnostic) ==
        UMI_STATUS_OK);
    assert(diagnostic.state == UMI_TEST_PLATFORM_BUILD_STATE_READY);
    return 0;
}

