/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/suite.h
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
#ifndef UMICOM_TEST_PLATFORM_SUITE_H
#define UMICOM_TEST_PLATFORM_SUITE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_SUITE_CAPACITY 4096U
#define UMI_TEST_PLATFORM_SUITE_API_VERSION 1U

/**
 * Represent the test platform suite snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformSuiteSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char project_id[128];
    char root_item_id[128];
    size_t test_count;
    int enabled;
    uint64_t revision;
} UmiTestPlatformSuiteSnapshot;

/**
 * Represent the test platform suite registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformSuiteRegistry UmiTestPlatformSuiteRegistry;

/**
 * Initialise test platform suite registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_platform_suite_registry_create(UmiTestPlatformSuiteRegistry **out_registry);
/**
 * Release or reset state held by test platform suite registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_suite_registry_destroy(UmiTestPlatformSuiteRegistry *registry);
/**
 * Provide the test platform suite registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_suite_registry_upsert(UmiTestPlatformSuiteRegistry *registry, const UmiTestPlatformSuiteSnapshot *item);
/**
 * Remove test platform suite registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_test_platform_suite_registry_remove(UmiTestPlatformSuiteRegistry *registry, const char *id);
/**
 * Find test platform suite registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_suite_registry_find(const UmiTestPlatformSuiteRegistry *registry, const char *id, UmiTestPlatformSuiteSnapshot *out_item);
/**
 * Find test platform suite registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_suite_registry_at(const UmiTestPlatformSuiteRegistry *registry, size_t index, UmiTestPlatformSuiteSnapshot *out_item);
/**
 * Return the number of records represented by test platform suite registry without
 * changing their state.
 */
size_t umi_test_platform_suite_registry_count(const UmiTestPlatformSuiteRegistry *registry);
/**
 * Provide the test platform suite registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_test_platform_suite_registry_revision(const UmiTestPlatformSuiteRegistry *registry);
/**
 * Release or reset state held by test platform suite registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_suite_registry_clear(UmiTestPlatformSuiteRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
