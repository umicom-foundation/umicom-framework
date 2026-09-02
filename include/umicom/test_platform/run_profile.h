/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/run_profile.h
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
#ifndef UMICOM_TEST_PLATFORM_RUN_PROFILE_H
#define UMICOM_TEST_PLATFORM_RUN_PROFILE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_RUN_PROFILE_CAPACITY 4096U
#define UMI_TEST_PLATFORM_RUN_PROFILE_API_VERSION 2U

/**
 * Represent the test platform run profile snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformRunProfileSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char mode[64];
    char configuration[512];
    char filter[512];
    int debug;
    int coverage;
    int default_profile;
    int include_disabled;
    int stop_on_failure;
    uint32_t repeat_count;
    uint32_t timeout_ms;
    uint64_t revision;
} UmiTestPlatformRunProfileSnapshot;

/**
 * Represent the test platform run profile registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformRunProfileRegistry UmiTestPlatformRunProfileRegistry;

/**
 * Initialise test platform run profile registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_run_profile_registry_create(UmiTestPlatformRunProfileRegistry **out_registry);
/**
 * Release or reset state held by test platform run profile registry so the same storage
 * can be reused safely.
 */
void umi_test_platform_run_profile_registry_destroy(UmiTestPlatformRunProfileRegistry *registry);
/**
 * Provide the test platform run profile registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_platform_run_profile_registry_upsert(UmiTestPlatformRunProfileRegistry *registry, const UmiTestPlatformRunProfileSnapshot *item);
/**
 * Remove test platform run profile registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_run_profile_registry_remove(UmiTestPlatformRunProfileRegistry *registry, const char *id);
/**
 * Find test platform run profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_run_profile_registry_find(const UmiTestPlatformRunProfileRegistry *registry, const char *id, UmiTestPlatformRunProfileSnapshot *out_item);
/**
 * Find test platform run profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_run_profile_registry_at(const UmiTestPlatformRunProfileRegistry *registry, size_t index, UmiTestPlatformRunProfileSnapshot *out_item);
/**
 * Return the number of records represented by test platform run profile registry without
 * changing their state.
 */
size_t umi_test_platform_run_profile_registry_count(const UmiTestPlatformRunProfileRegistry *registry);
/**
 * Provide the test platform run profile registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_test_platform_run_profile_registry_revision(const UmiTestPlatformRunProfileRegistry *registry);
/**
 * Release or reset state held by test platform run profile registry so the same storage
 * can be reused safely.
 */
void umi_test_platform_run_profile_registry_clear(UmiTestPlatformRunProfileRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
