/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/result.h
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
#ifndef UMICOM_TEST_PLATFORM_RESULT_H
#define UMICOM_TEST_PLATFORM_RESULT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_RESULT_CAPACITY 4096U
#define UMI_TEST_PLATFORM_RESULT_API_VERSION 1U

typedef struct UmiTestPlatformResultSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char item_id[128];
    char message[1024];
    double duration_ms;
    int outcome;
    int flaky;
    uint64_t revision;
} UmiTestPlatformResultSnapshot;

typedef struct UmiTestPlatformResultRegistry UmiTestPlatformResultRegistry;

UmiStatus umi_test_platform_result_registry_create(UmiTestPlatformResultRegistry **out_registry);
void umi_test_platform_result_registry_destroy(UmiTestPlatformResultRegistry *registry);
UmiStatus umi_test_platform_result_registry_upsert(UmiTestPlatformResultRegistry *registry, const UmiTestPlatformResultSnapshot *item);
UmiStatus umi_test_platform_result_registry_remove(UmiTestPlatformResultRegistry *registry, const char *id);
UmiStatus umi_test_platform_result_registry_find(const UmiTestPlatformResultRegistry *registry, const char *id, UmiTestPlatformResultSnapshot *out_item);
UmiStatus umi_test_platform_result_registry_at(const UmiTestPlatformResultRegistry *registry, size_t index, UmiTestPlatformResultSnapshot *out_item);
size_t umi_test_platform_result_registry_count(const UmiTestPlatformResultRegistry *registry);
uint64_t umi_test_platform_result_registry_revision(const UmiTestPlatformResultRegistry *registry);
void umi_test_platform_result_registry_clear(UmiTestPlatformResultRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
