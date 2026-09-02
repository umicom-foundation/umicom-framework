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

/**
 * Represent the test platform output snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the test platform output registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformOutputRegistry UmiTestPlatformOutputRegistry;

/**
 * Initialise test platform output registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_platform_output_registry_create(UmiTestPlatformOutputRegistry **out_registry);
/**
 * Release or reset state held by test platform output registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_output_registry_destroy(UmiTestPlatformOutputRegistry *registry);
/**
 * Provide the test platform output registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_output_registry_upsert(UmiTestPlatformOutputRegistry *registry, const UmiTestPlatformOutputSnapshot *item);
/**
 * Remove test platform output registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_test_platform_output_registry_remove(UmiTestPlatformOutputRegistry *registry, const char *id);
/**
 * Find test platform output registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_output_registry_find(const UmiTestPlatformOutputRegistry *registry, const char *id, UmiTestPlatformOutputSnapshot *out_item);
/**
 * Find test platform output registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_output_registry_at(const UmiTestPlatformOutputRegistry *registry, size_t index, UmiTestPlatformOutputSnapshot *out_item);
/**
 * Return the number of records represented by test platform output registry without
 * changing their state.
 */
size_t umi_test_platform_output_registry_count(const UmiTestPlatformOutputRegistry *registry);
/**
 * Provide the test platform output registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_test_platform_output_registry_revision(const UmiTestPlatformOutputRegistry *registry);
/**
 * Release or reset state held by test platform output registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_output_registry_clear(UmiTestPlatformOutputRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
