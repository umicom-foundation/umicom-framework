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

typedef struct UmiTestPlatformSuiteRegistry UmiTestPlatformSuiteRegistry;

UmiStatus umi_test_platform_suite_registry_create(UmiTestPlatformSuiteRegistry **out_registry);
void umi_test_platform_suite_registry_destroy(UmiTestPlatformSuiteRegistry *registry);
UmiStatus umi_test_platform_suite_registry_upsert(UmiTestPlatformSuiteRegistry *registry, const UmiTestPlatformSuiteSnapshot *item);
UmiStatus umi_test_platform_suite_registry_remove(UmiTestPlatformSuiteRegistry *registry, const char *id);
UmiStatus umi_test_platform_suite_registry_find(const UmiTestPlatformSuiteRegistry *registry, const char *id, UmiTestPlatformSuiteSnapshot *out_item);
UmiStatus umi_test_platform_suite_registry_at(const UmiTestPlatformSuiteRegistry *registry, size_t index, UmiTestPlatformSuiteSnapshot *out_item);
size_t umi_test_platform_suite_registry_count(const UmiTestPlatformSuiteRegistry *registry);
uint64_t umi_test_platform_suite_registry_revision(const UmiTestPlatformSuiteRegistry *registry);
void umi_test_platform_suite_registry_clear(UmiTestPlatformSuiteRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
