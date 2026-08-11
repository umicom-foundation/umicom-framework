/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_artifacts.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/artifact_set.h"
int main(void) {
    UmiArtifactSet set;
    UmiDeliveryArtifact a;
    umi_artifact_set_init(&set);
    assert(umi_delivery_artifact_init(&a, "studio.exe", UMI_ARTIFACT_EXECUTABLE, "bin/studio.exe") == UMI_STATUS_OK);
    a.size_bytes = 42U;
    assert(umi_artifact_set_add(&set, &a) == UMI_STATUS_OK);
    assert(umi_artifact_set_find(&set, "studio.exe") != 0);
    assert(umi_artifact_set_total_size(&set) == 42U);
    return 0;
}
