/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/coverage.h
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

/**
 * Represent the test platform coverage snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the test platform coverage registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformCoverageRegistry UmiTestPlatformCoverageRegistry;

/**
 * Initialise test platform coverage registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_coverage_registry_create(UmiTestPlatformCoverageRegistry **out_registry);
/**
 * Release or reset state held by test platform coverage registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_coverage_registry_destroy(UmiTestPlatformCoverageRegistry *registry);
/**
 * Provide the test platform coverage registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_coverage_registry_upsert(UmiTestPlatformCoverageRegistry *registry, const UmiTestPlatformCoverageSnapshot *item);
/**
 * Remove test platform coverage registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_coverage_registry_remove(UmiTestPlatformCoverageRegistry *registry, const char *id);
/**
 * Find test platform coverage registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_coverage_registry_find(const UmiTestPlatformCoverageRegistry *registry, const char *id, UmiTestPlatformCoverageSnapshot *out_item);
/**
 * Find test platform coverage registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_coverage_registry_at(const UmiTestPlatformCoverageRegistry *registry, size_t index, UmiTestPlatformCoverageSnapshot *out_item);
/**
 * Return the number of records represented by test platform coverage registry without
 * changing their state.
 */
size_t umi_test_platform_coverage_registry_count(const UmiTestPlatformCoverageRegistry *registry);
/**
 * Provide the test platform coverage registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_test_platform_coverage_registry_revision(const UmiTestPlatformCoverageRegistry *registry);
/**
 * Release or reset state held by test platform coverage registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_coverage_registry_clear(UmiTestPlatformCoverageRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
