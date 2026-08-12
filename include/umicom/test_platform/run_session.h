/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/run_session.h
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
#ifndef UMICOM_TEST_PLATFORM_RUN_SESSION_H
#define UMICOM_TEST_PLATFORM_RUN_SESSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_RUN_SESSION_CAPACITY 4096U
#define UMI_TEST_PLATFORM_RUN_SESSION_API_VERSION 1U

typedef struct UmiTestPlatformRunSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char profile_id[128];
    char suite_id[128];
    uint64_t started_at;
    uint64_t finished_at;
    size_t total;
    size_t passed;
    size_t failed;
    size_t skipped;
    int state;
    uint64_t revision;
} UmiTestPlatformRunSessionSnapshot;

typedef struct UmiTestPlatformRunSessionRegistry UmiTestPlatformRunSessionRegistry;

UmiStatus umi_test_platform_run_session_registry_create(UmiTestPlatformRunSessionRegistry **out_registry);
void umi_test_platform_run_session_registry_destroy(UmiTestPlatformRunSessionRegistry *registry);
UmiStatus umi_test_platform_run_session_registry_upsert(UmiTestPlatformRunSessionRegistry *registry, const UmiTestPlatformRunSessionSnapshot *item);
UmiStatus umi_test_platform_run_session_registry_remove(UmiTestPlatformRunSessionRegistry *registry, const char *id);
UmiStatus umi_test_platform_run_session_registry_find(const UmiTestPlatformRunSessionRegistry *registry, const char *id, UmiTestPlatformRunSessionSnapshot *out_item);
UmiStatus umi_test_platform_run_session_registry_at(const UmiTestPlatformRunSessionRegistry *registry, size_t index, UmiTestPlatformRunSessionSnapshot *out_item);
size_t umi_test_platform_run_session_registry_count(const UmiTestPlatformRunSessionRegistry *registry);
uint64_t umi_test_platform_run_session_registry_revision(const UmiTestPlatformRunSessionRegistry *registry);
void umi_test_platform_run_session_registry_clear(UmiTestPlatformRunSessionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
