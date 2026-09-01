/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/output.h
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
#ifndef UMICOM_TEST_PLATFORM_OUTPUT_H
#define UMICOM_TEST_PLATFORM_OUTPUT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_OUTPUT_CAPACITY 4096U
#define UMI_TEST_PLATFORM_OUTPUT_API_VERSION 1U

typedef struct UmiTestPlatformOutputSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char item_id[128];
    char stream[64];
    char text[2048];
    uint64_t timestamp;
    uint64_t revision;
} UmiTestPlatformOutputSnapshot;

typedef struct UmiTestPlatformOutputRegistry UmiTestPlatformOutputRegistry;

UmiStatus umi_test_platform_output_registry_create(UmiTestPlatformOutputRegistry **out_registry);
void umi_test_platform_output_registry_destroy(UmiTestPlatformOutputRegistry *registry);
UmiStatus umi_test_platform_output_registry_upsert(UmiTestPlatformOutputRegistry *registry, const UmiTestPlatformOutputSnapshot *item);
UmiStatus umi_test_platform_output_registry_remove(UmiTestPlatformOutputRegistry *registry, const char *id);
UmiStatus umi_test_platform_output_registry_find(const UmiTestPlatformOutputRegistry *registry, const char *id, UmiTestPlatformOutputSnapshot *out_item);
UmiStatus umi_test_platform_output_registry_at(const UmiTestPlatformOutputRegistry *registry, size_t index, UmiTestPlatformOutputSnapshot *out_item);
size_t umi_test_platform_output_registry_count(const UmiTestPlatformOutputRegistry *registry);
uint64_t umi_test_platform_output_registry_revision(const UmiTestPlatformOutputRegistry *registry);
void umi_test_platform_output_registry_clear(UmiTestPlatformOutputRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
