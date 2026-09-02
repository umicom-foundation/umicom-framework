/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_artifact.c
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
#include "umicom/test_platform/build_readiness/artifact.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTestPlatformBuildArtifact artifact;
    assert(umi_test_platform_build_artifact_init(&artifact, "studio",
        "studio-test-target", "studio.test", "studio;smoke",
        "windows-ucrt64-debug", true) == UMI_STATUS_OK);
    assert(umi_test_platform_build_artifact_validate(&artifact) == UMI_STATUS_OK);
    assert(strcmp(artifact.target_name, "studio-test-target") == 0);
    assert(artifact.required);
    artifact.test_name[0] = '\0';
    assert(umi_test_platform_build_artifact_validate(&artifact) ==
        UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

