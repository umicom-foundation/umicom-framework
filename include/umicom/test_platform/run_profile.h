/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/run_profile.h
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
#ifndef UMICOM_TEST_PLATFORM_RUN_PROFILE_H
#define UMICOM_TEST_PLATFORM_RUN_PROFILE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_RUN_PROFILE_CAPACITY 4096U
#define UMI_TEST_PLATFORM_RUN_PROFILE_API_VERSION 1U

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
    uint64_t revision;
} UmiTestPlatformRunProfileSnapshot;

typedef struct UmiTestPlatformRunProfileRegistry UmiTestPlatformRunProfileRegistry;

UmiStatus umi_test_platform_run_profile_registry_create(UmiTestPlatformRunProfileRegistry **out_registry);
void umi_test_platform_run_profile_registry_destroy(UmiTestPlatformRunProfileRegistry *registry);
UmiStatus umi_test_platform_run_profile_registry_upsert(UmiTestPlatformRunProfileRegistry *registry, const UmiTestPlatformRunProfileSnapshot *item);
UmiStatus umi_test_platform_run_profile_registry_remove(UmiTestPlatformRunProfileRegistry *registry, const char *id);
UmiStatus umi_test_platform_run_profile_registry_find(const UmiTestPlatformRunProfileRegistry *registry, const char *id, UmiTestPlatformRunProfileSnapshot *out_item);
UmiStatus umi_test_platform_run_profile_registry_at(const UmiTestPlatformRunProfileRegistry *registry, size_t index, UmiTestPlatformRunProfileSnapshot *out_item);
size_t umi_test_platform_run_profile_registry_count(const UmiTestPlatformRunProfileRegistry *registry);
uint64_t umi_test_platform_run_profile_registry_revision(const UmiTestPlatformRunProfileRegistry *registry);
void umi_test_platform_run_profile_registry_clear(UmiTestPlatformRunProfileRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
