/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/item.h
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
#ifndef UMICOM_TEST_PLATFORM_ITEM_H
#define UMICOM_TEST_PLATFORM_ITEM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Retained compatibility window for existing history consumers. Registry
 * storage is now dynamically allocated, with a separate explicit hard limit. */
#define UMI_TEST_PLATFORM_ITEM_CAPACITY 4096U
#define UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY 65536U
#define UMI_TEST_PLATFORM_ITEM_API_VERSION 2U

/**
 * Represent the test platform item snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char parent_id[128];
    char suite_id[128];
    char name[256];
    char uri[1024];
    char source_uri[1024];
    char framework[128];
    char kind[64];
    char labels[512];
    char command[1024];
    char working_directory[1024];
    uint32_t source_line;
    uint32_t timeout_ms;
    int enabled;
    int discovered;
    int last_outcome;
    double last_duration_ms;
    uint64_t revision;
} UmiTestPlatformItemSnapshot;

/**
 * Represent the test platform item registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformItemRegistry UmiTestPlatformItemRegistry;

/* The registry owns copied records; callers retain ownership of every snapshot
 * passed to upsert. Revisions make explorer refreshes cheap to compare. */
UmiStatus umi_test_platform_item_registry_create(UmiTestPlatformItemRegistry **out_registry);
/**
 * Release or reset state held by test platform item registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_item_registry_destroy(UmiTestPlatformItemRegistry *registry);
/**
 * Provide the test platform item registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_item_registry_upsert(UmiTestPlatformItemRegistry *registry, const UmiTestPlatformItemSnapshot *item);
/**
 * Remove test platform item registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_test_platform_item_registry_remove(UmiTestPlatformItemRegistry *registry, const char *id);
/**
 * Find test platform item registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_item_registry_find(const UmiTestPlatformItemRegistry *registry, const char *id, UmiTestPlatformItemSnapshot *out_item);
/**
 * Find test platform item registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_item_registry_at(const UmiTestPlatformItemRegistry *registry, size_t index, UmiTestPlatformItemSnapshot *out_item);
/**
 * Return the number of records represented by test platform item registry without changing
 * their state.
 */
size_t umi_test_platform_item_registry_count(const UmiTestPlatformItemRegistry *registry);
/**
 * Provide the test platform item registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_test_platform_item_registry_revision(const UmiTestPlatformItemRegistry *registry);
/**
 * Release or reset state held by test platform item registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_item_registry_clear(UmiTestPlatformItemRegistry *registry);

/** Create an independent, exact copy for a staged catalogue transaction.
 * Existing records and revisions are preserved. The caller owns the clone.
 * These registries are single-owner objects: synchronise all access externally.
 */
UmiStatus UmiTestPlatformItemRegistryClone(const UmiTestPlatformItemRegistry *source,
    UmiTestPlatformItemRegistry **outRegistry);
/** Exchange complete valid registry states without allocation or callbacks.
 * Use only on the owning thread, after every staged operation has succeeded.
 * Existing registry object addresses remain valid; borrowed private storage does
 * not escape through this API. A NULL argument or the same object is a no-op.
 */
void UmiTestPlatformItemRegistrySwap(UmiTestPlatformItemRegistry *left,
    UmiTestPlatformItemRegistry *right);

/** Remove a suite's item records in one stable compaction. Other suites keep
 * their order and records. Intended for an unpublished clone during refresh.
 */
UmiStatus UmiTestPlatformItemRegistryRemoveSuite(UmiTestPlatformItemRegistry *registry,
    const char *suiteId);

#ifdef __cplusplus
}
#endif

#endif
