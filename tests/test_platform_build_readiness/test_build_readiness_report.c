/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_report.c
 * PURPOSE: Focused regression for the Framework build-readiness platform.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/test_platform/build_readiness/readiness_report.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTestPlatformBuildArtifact ready;
    UmiTestPlatformBuildArtifact missing;
    UmiTestPlatformBuildInventory inventory;
    UmiTestPlatformBuildReadinessReport report;
    assert(umi_test_platform_build_artifact_init(&ready, "studio", "a",
        "studio.a", "studio", "windows-ucrt64-debug", true) == UMI_STATUS_OK);
    assert(umi_test_platform_build_artifact_init(&missing, "studio", "b",
        "studio.b", "studio", "windows-ucrt64-debug", true) == UMI_STATUS_OK);
    umi_test_platform_build_inventory_init(&inventory);
    assert(umi_test_platform_build_inventory_record(&inventory, &ready,
        UMI_TEST_PLATFORM_BUILD_STATE_READY, "ready") == UMI_STATUS_OK);
    assert(umi_test_platform_build_inventory_record(&inventory, &missing,
        UMI_TEST_PLATFORM_BUILD_STATE_ARTIFACT_MISSING, "missing") ==
        UMI_STATUS_OK);
    assert(umi_test_platform_build_readiness_report_create(&inventory, &report)
        == UMI_STATUS_OK);
    assert(report.total_count == 2U && report.ready_count == 1U);
    assert(report.missing_count == 1U && !report.ready_to_run);
    return 0;
}

