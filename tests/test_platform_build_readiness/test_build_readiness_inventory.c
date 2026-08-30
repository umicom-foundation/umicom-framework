/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_inventory.c
 * PURPOSE: Focused regression for the Framework build-readiness platform.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/test_platform/build_readiness/inventory.h"
int main(void) {
    UmiTestPlatformBuildArtifact artifact;
    UmiTestPlatformBuildInventory inventory;
    assert(umi_test_platform_build_artifact_init(&artifact, "studio", "target",
        "studio.test", "studio", "windows-ucrt64-debug", true) == UMI_STATUS_OK);
    umi_test_platform_build_inventory_init(&inventory);
    assert(umi_test_platform_build_inventory_record(&inventory, &artifact,
        UMI_TEST_PLATFORM_BUILD_STATE_ARTIFACT_MISSING, "missing") ==
        UMI_STATUS_OK);
    assert(inventory.count == 1U);
    assert(umi_test_platform_build_inventory_record(&inventory, &artifact,
        UMI_TEST_PLATFORM_BUILD_STATE_READY, "ready") == UMI_STATUS_OK);
    assert(inventory.count == 1U);
    assert(umi_test_platform_build_inventory_find_test(&inventory,
        "studio.test")->state == UMI_TEST_PLATFORM_BUILD_STATE_READY);
    return 0;
}

