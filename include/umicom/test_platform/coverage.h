/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/coverage.h
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
#ifndef UMICOM_TEST_PLATFORM_COVERAGE_H
#define UMICOM_TEST_PLATFORM_COVERAGE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_COVERAGE_CAPACITY 4096U
#define UMI_TEST_PLATFORM_COVERAGE_API_VERSION 1U

typedef struct UmiTestPlatformCoverageSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char uri[1024];
    uint64_t lines_total;
    uint64_t lines_covered;
    uint64_t branches_total;
    uint64_t branches_covered;
    uint64_t revision;
} UmiTestPlatformCoverageSnapshot;

typedef struct UmiTestPlatformCoverageRegistry UmiTestPlatformCoverageRegistry;

UmiStatus umi_test_platform_coverage_registry_create(UmiTestPlatformCoverageRegistry **out_registry);
void umi_test_platform_coverage_registry_destroy(UmiTestPlatformCoverageRegistry *registry);
UmiStatus umi_test_platform_coverage_registry_upsert(UmiTestPlatformCoverageRegistry *registry, const UmiTestPlatformCoverageSnapshot *item);
UmiStatus umi_test_platform_coverage_registry_remove(UmiTestPlatformCoverageRegistry *registry, const char *id);
UmiStatus umi_test_platform_coverage_registry_find(const UmiTestPlatformCoverageRegistry *registry, const char *id, UmiTestPlatformCoverageSnapshot *out_item);
UmiStatus umi_test_platform_coverage_registry_at(const UmiTestPlatformCoverageRegistry *registry, size_t index, UmiTestPlatformCoverageSnapshot *out_item);
size_t umi_test_platform_coverage_registry_count(const UmiTestPlatformCoverageRegistry *registry);
uint64_t umi_test_platform_coverage_registry_revision(const UmiTestPlatformCoverageRegistry *registry);
void umi_test_platform_coverage_registry_clear(UmiTestPlatformCoverageRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
