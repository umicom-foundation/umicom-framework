/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/item.h
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
#ifndef UMICOM_TEST_PLATFORM_ITEM_H
#define UMICOM_TEST_PLATFORM_ITEM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_ITEM_CAPACITY 4096U
#define UMI_TEST_PLATFORM_ITEM_API_VERSION 1U

typedef struct UmiTestPlatformItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char parent_id[128];
    char name[256];
    char uri[1024];
    char framework[128];
    char kind[64];
    int enabled;
    int discovered;
    uint64_t revision;
} UmiTestPlatformItemSnapshot;

typedef struct UmiTestPlatformItemRegistry UmiTestPlatformItemRegistry;

UmiStatus umi_test_platform_item_registry_create(UmiTestPlatformItemRegistry **out_registry);
void umi_test_platform_item_registry_destroy(UmiTestPlatformItemRegistry *registry);
UmiStatus umi_test_platform_item_registry_upsert(UmiTestPlatformItemRegistry *registry, const UmiTestPlatformItemSnapshot *item);
UmiStatus umi_test_platform_item_registry_remove(UmiTestPlatformItemRegistry *registry, const char *id);
UmiStatus umi_test_platform_item_registry_find(const UmiTestPlatformItemRegistry *registry, const char *id, UmiTestPlatformItemSnapshot *out_item);
UmiStatus umi_test_platform_item_registry_at(const UmiTestPlatformItemRegistry *registry, size_t index, UmiTestPlatformItemSnapshot *out_item);
size_t umi_test_platform_item_registry_count(const UmiTestPlatformItemRegistry *registry);
uint64_t umi_test_platform_item_registry_revision(const UmiTestPlatformItemRegistry *registry);
void umi_test_platform_item_registry_clear(UmiTestPlatformItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
