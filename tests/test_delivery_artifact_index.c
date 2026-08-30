/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_artifact_index.c
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
#include "umicom/delivery/artifact_index.h"
int main(void) {
    UmiArtifactIndex index;
    UmiDeliveryArtifact artifact;
    umi_artifact_index_init(&index);
    assert(umi_delivery_artifact_init(&artifact, "a", UMI_ARTIFACT_ARCHIVE, "a.zip") == UMI_STATUS_OK);
    assert(umi_artifact_index_add(&index, &artifact) == UMI_STATUS_OK);
    assert(umi_artifact_index_get(&index, "a") != 0);
    return 0;
}
