/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/inventory.h
 * PURPOSE: Retain bounded observations of required CTest executables.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_INVENTORY_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_INVENTORY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/build_readiness/artifact.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_BUILD_INVENTORY_CAPACITY 128U

typedef struct UmiTestPlatformBuildObservation {
    const UmiTestPlatformBuildArtifact *artifact;
    UmiTestPlatformBuildReadinessState state;
    char message[256];
} UmiTestPlatformBuildObservation;

typedef struct UmiTestPlatformBuildInventory {
    UmiTestPlatformBuildObservation
        items[UMI_TEST_PLATFORM_BUILD_INVENTORY_CAPACITY];
    size_t count;
} UmiTestPlatformBuildInventory;

void umi_test_platform_build_inventory_init(
    UmiTestPlatformBuildInventory *inventory);
UmiStatus umi_test_platform_build_inventory_record(
    UmiTestPlatformBuildInventory *inventory,
    const UmiTestPlatformBuildArtifact *artifact,
    UmiTestPlatformBuildReadinessState state,
    const char *message);
const UmiTestPlatformBuildObservation *
umi_test_platform_build_inventory_find_test(
    const UmiTestPlatformBuildInventory *inventory,
    const char *test_name);

#ifdef __cplusplus
}
#endif
#endif
