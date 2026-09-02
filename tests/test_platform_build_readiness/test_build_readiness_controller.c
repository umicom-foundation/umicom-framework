/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_controller.c
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
#include "umicom/test_platform/build_readiness/controller.h"
/*
 * Exercise probe and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus probe(const UmiTestPlatformBuildArtifact *artifact,
                       UmiTestPlatformBuildDiagnosticInput *input,
                       void *user_data) {
    const char *missing = (const char *)user_data;
    input->configure_completed = true;
    input->target_enabled = true;
    input->build_completed = true;
    input->test_registered = true;
    input->executable_exists = strcmp(artifact->target_name, missing) != 0;
    return UMI_STATUS_OK;
}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTestPlatformBuildInventory inventory;
    UmiTestPlatformBuildReadinessReport report;
    assert(umi_test_platform_build_readiness_assess_product("studio", probe,
        "umicom-studio-experience-centre-test", &inventory, &report) ==
        UMI_STATUS_OK);
    assert(inventory.count == 9U);
    assert(report.missing_count == 1U && !report.ready_to_run);
    assert(umi_test_platform_build_readiness_assess_product("missing", probe,
        NULL, &inventory, &report) == UMI_STATUS_NOT_FOUND);
    return 0;
}

