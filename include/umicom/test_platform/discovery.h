/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/discovery.h
 *
 * PURPOSE:
 *   Define a reusable test-explorer and test-run record independent of any single test framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_TEST_PLATFORM_DISCOVERY_H
#define UMICOM_TEST_PLATFORM_DISCOVERY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_DISCOVERY_CAPACITY 4096U
#define UMI_TEST_PLATFORM_DISCOVERY_API_VERSION 2U

/**
 * Represent the test platform discovery snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformDiscoverySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char provider[128];
    char root_uri[1024];
    char configuration[128];
    size_t discovered_count;
    int state;
    int automatic;
    int metadata_supported;
    uint64_t revision;
} UmiTestPlatformDiscoverySnapshot;

/**
 * Represent the test platform discovery registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformDiscoveryRegistry UmiTestPlatformDiscoveryRegistry;

/**
 * Initialise test platform discovery registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_discovery_registry_create(UmiTestPlatformDiscoveryRegistry **out_registry);
/**
 * Release or reset state held by test platform discovery registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_discovery_registry_destroy(UmiTestPlatformDiscoveryRegistry *registry);
/**
 * Provide the test platform discovery registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_platform_discovery_registry_upsert(UmiTestPlatformDiscoveryRegistry *registry, const UmiTestPlatformDiscoverySnapshot *item);
/**
 * Remove test platform discovery registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_discovery_registry_remove(UmiTestPlatformDiscoveryRegistry *registry, const char *id);
/**
 * Find test platform discovery registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_discovery_registry_find(const UmiTestPlatformDiscoveryRegistry *registry, const char *id, UmiTestPlatformDiscoverySnapshot *out_item);
/**
 * Find test platform discovery registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_discovery_registry_at(const UmiTestPlatformDiscoveryRegistry *registry, size_t index, UmiTestPlatformDiscoverySnapshot *out_item);
/**
 * Return the number of records represented by test platform discovery registry without
 * changing their state.
 */
size_t umi_test_platform_discovery_registry_count(const UmiTestPlatformDiscoveryRegistry *registry);
/**
 * Provide the test platform discovery registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_test_platform_discovery_registry_revision(const UmiTestPlatformDiscoveryRegistry *registry);
/**
 * Release or reset state held by test platform discovery registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_discovery_registry_clear(UmiTestPlatformDiscoveryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
