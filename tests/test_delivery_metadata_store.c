/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_metadata_store.c
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

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "value_archive/construction_cases.h"
#include <string.h>
#include "umicom/delivery/metadata.h"
#include "umicom/delivery/store.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted generation value. */
static int UmiGenerationConstructionCases(void)
{
    UmiGeneration *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_generation_init(value, 1U, "r", "root") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->release_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_generation_init(value, 1U, oversized, "root") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_generation_init(value, 1U, value->release_id, "root") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->release_id, "r") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->root) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_generation_init(value, 1U, "r", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_generation_init(value, 1U, "r", value->root) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->root, "root") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiGenerationConstructionCases() != 0) return 1;

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
