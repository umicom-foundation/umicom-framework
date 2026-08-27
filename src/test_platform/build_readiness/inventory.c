/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/inventory.c
 * PURPOSE: Store bounded and replaceable executable-readiness observations.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/inventory.h"

#include <string.h>

static void copy_message(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

void umi_test_platform_build_inventory_init(
    UmiTestPlatformBuildInventory *inventory)
{
    if (inventory != NULL) (void)memset(inventory, 0, sizeof(*inventory));
}

UmiStatus umi_test_platform_build_inventory_record(
    UmiTestPlatformBuildInventory *inventory,
    const UmiTestPlatformBuildArtifact *artifact,
    UmiTestPlatformBuildReadinessState state, const char *message)
{
    size_t index;
    UmiTestPlatformBuildObservation *observation;
    if (inventory == NULL ||
        umi_test_platform_build_artifact_validate(artifact) != UMI_STATUS_OK ||
        state < UMI_TEST_PLATFORM_BUILD_STATE_UNKNOWN ||
        state > UMI_TEST_PLATFORM_BUILD_STATE_ARTIFACT_MISSING)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < inventory->count; ++index) {
        if (strcmp(inventory->items[index].artifact->test_name,
                   artifact->test_name) == 0) break;
    }
    if (index == inventory->count) {
        if (inventory->count >= UMI_TEST_PLATFORM_BUILD_INVENTORY_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        inventory->count++;
    }
    observation = &inventory->items[index];
    observation->artifact = artifact;
    observation->state = state;
    copy_message(observation->message, sizeof(observation->message), message);
    return UMI_STATUS_OK;
}

const UmiTestPlatformBuildObservation *
umi_test_platform_build_inventory_find_test(
    const UmiTestPlatformBuildInventory *inventory, const char *test_name)
{
    size_t index;
    if (inventory == NULL || test_name == NULL) return NULL;
    for (index = 0U; index < inventory->count; ++index) {
        if (strcmp(inventory->items[index].artifact->test_name, test_name) == 0)
            return &inventory->items[index];
    }
    return NULL;
}
