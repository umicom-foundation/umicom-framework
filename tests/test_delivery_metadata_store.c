/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_metadata_store.c
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
#include <string.h>
#include "umicom/delivery/metadata.h"
#include "umicom/delivery/store.h"
int main(void) {
    UmiDeliveryMetadata metadata;
    UmiGenerationStore store;
    UmiGeneration generation;
    umi_delivery_metadata_init(&metadata);
    assert(umi_delivery_metadata_set(&metadata, "host", "builder") == UMI_STATUS_OK);
    assert(strcmp(umi_delivery_metadata_get(&metadata, "host"), "builder") == 0);
    umi_generation_store_init(&store);
    assert(umi_generation_init(&generation, 1U, "r", "root") == UMI_STATUS_OK);
    umi_generation_mark_active(&generation, 1);
    assert(umi_generation_store_add(&store, &generation) == UMI_STATUS_OK);
    assert(umi_generation_store_active(&store) != 0);
    return 0;
}
