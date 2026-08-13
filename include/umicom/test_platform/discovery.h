/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/discovery.h
 *
 * PURPOSE:
 *   Define a reusable test-explorer and test-run record independent of any single test framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiTestPlatformDiscoveryRegistry UmiTestPlatformDiscoveryRegistry;

UmiStatus umi_test_platform_discovery_registry_create(UmiTestPlatformDiscoveryRegistry **out_registry);
void umi_test_platform_discovery_registry_destroy(UmiTestPlatformDiscoveryRegistry *registry);
UmiStatus umi_test_platform_discovery_registry_upsert(UmiTestPlatformDiscoveryRegistry *registry, const UmiTestPlatformDiscoverySnapshot *item);
UmiStatus umi_test_platform_discovery_registry_remove(UmiTestPlatformDiscoveryRegistry *registry, const char *id);
UmiStatus umi_test_platform_discovery_registry_find(const UmiTestPlatformDiscoveryRegistry *registry, const char *id, UmiTestPlatformDiscoverySnapshot *out_item);
UmiStatus umi_test_platform_discovery_registry_at(const UmiTestPlatformDiscoveryRegistry *registry, size_t index, UmiTestPlatformDiscoverySnapshot *out_item);
size_t umi_test_platform_discovery_registry_count(const UmiTestPlatformDiscoveryRegistry *registry);
uint64_t umi_test_platform_discovery_registry_revision(const UmiTestPlatformDiscoveryRegistry *registry);
void umi_test_platform_discovery_registry_clear(UmiTestPlatformDiscoveryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
