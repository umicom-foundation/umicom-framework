/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_stage_package.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/stage.h"
#include "umicom/delivery/directory_package.h"
int main(void) {
    UmiDeliveryStageRecord stage;
    UmiPackageSpec spec;
    UmiPackageProvider provider;
    UmiPackageResult result;
    assert(umi_delivery_stage_init(&stage, "stage/root") == UMI_STATUS_OK);
    umi_delivery_stage_add_file(&stage, 100U);
    umi_delivery_stage_complete(&stage);
    assert(stage.complete && stage.total_bytes == 100U);
    assert(umi_package_spec_init(&spec, "portable", UMI_PACKAGE_DIRECTORY, "stage/root", "dist/studio") == UMI_STATUS_OK);
    assert(umi_directory_package_provider(&provider) == UMI_STATUS_OK);
    assert(umi_package_provider_create(&provider, &spec, &result) == UMI_STATUS_OK);
    assert(result.succeeded);
    return 0;
}
