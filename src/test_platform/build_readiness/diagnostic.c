/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/diagnostic.c
 * PURPOSE: Explain Not Run without misreporting it as a test assertion failure.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/diagnostic.h"

#include <stdio.h>
#include <string.h>

/* Provide the set diagnostic operation used by this module and its client applications. */
static void set_diagnostic(UmiTestPlatformBuildDiagnostic *diagnostic,
                           UmiTestPlatformBuildReadinessState state,
                           const char *summary, const char *command_id)
{
    diagnostic->state = state;
    (void)snprintf(diagnostic->summary, sizeof(diagnostic->summary), "%s",
                   summary);
    (void)snprintf(diagnostic->remediation_command_id,
                   sizeof(diagnostic->remediation_command_id), "%s",
                   command_id);
}

/*
 * Provide the test platform build diagnostic classify operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_platform_build_diagnostic_classify(
    const UmiTestPlatformBuildDiagnosticInput *input,
    UmiTestPlatformBuildDiagnostic *diagnostic)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || diagnostic == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(diagnostic, 0, sizeof(*diagnostic));
    /* Apply this branch only when its contract condition is satisfied. */
    if (!input->configure_completed) {
        set_diagnostic(diagnostic,
            UMI_TEST_PLATFORM_BUILD_STATE_CONFIGURE_FAILED,
            "CMake configuration did not complete; CTest data is stale.",
            "test.configure.fresh");
    } else /* Apply this operation only while the related capability or state is available. */ if (!input->target_enabled) {
        set_diagnostic(diagnostic,
            UMI_TEST_PLATFORM_BUILD_STATE_TARGET_DISABLED,
            "The product or validation target is disabled by this preset.",
            "test.select.product-preset");
    } else /* Apply this branch only when its contract condition is satisfied. */ if (!input->build_completed) {
        set_diagnostic(diagnostic, UMI_TEST_PLATFORM_BUILD_STATE_BUILD_FAILED,
            "The build stopped before required test executables were linked.",
            "test.build.required-artifacts");
    } else /* Apply this branch only when its contract condition is satisfied. */ if (!input->test_registered) {
        set_diagnostic(diagnostic,
            UMI_TEST_PLATFORM_BUILD_STATE_TEST_UNREGISTERED,
            "The executable target is not registered in the CTest catalogue.",
            "test.inspect.registration");
    } else /* Apply this branch only when its contract condition is satisfied. */ if (!input->executable_exists) {
        set_diagnostic(diagnostic,
            UMI_TEST_PLATFORM_BUILD_STATE_ARTIFACT_MISSING,
            "CTest is registered but its executable artifact is missing.",
            "test.build.required-artifacts");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        set_diagnostic(diagnostic, UMI_TEST_PLATFORM_BUILD_STATE_READY,
            "The CTest executable is available and ready to run.",
            "test.run.focused");
    }
    return UMI_STATUS_OK;
}
