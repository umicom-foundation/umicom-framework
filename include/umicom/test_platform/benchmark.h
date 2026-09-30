/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/benchmark.h
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
#ifndef UMICOM_TEST_PLATFORM_BENCHMARK_H
#define UMICOM_TEST_PLATFORM_BENCHMARK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_BENCHMARK_CAPACITY 4096U
#define UMI_TEST_PLATFORM_BENCHMARK_API_VERSION 1U

/**
 * Represent the test platform benchmark snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformBenchmarkSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char result_id[128];
    char metric[128];
    char unit[64];
    double value;
    double baseline;
    double tolerance;
    int regression;
    uint64_t revision;
} UmiTestPlatformBenchmarkSnapshot;

/**
 * Represent the test platform benchmark registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformBenchmarkRegistry UmiTestPlatformBenchmarkRegistry;

/**
 * Initialise test platform benchmark registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_benchmark_registry_create(UmiTestPlatformBenchmarkRegistry **out_registry);
/**
 * Release or reset state held by test platform benchmark registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_benchmark_registry_destroy(UmiTestPlatformBenchmarkRegistry *registry);
/**
 * Provide the test platform benchmark registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_platform_benchmark_registry_upsert(UmiTestPlatformBenchmarkRegistry *registry, const UmiTestPlatformBenchmarkSnapshot *item);
/**
 * Remove test platform benchmark registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_benchmark_registry_remove(UmiTestPlatformBenchmarkRegistry *registry, const char *id);
/**
 * Find test platform benchmark registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_benchmark_registry_find(const UmiTestPlatformBenchmarkRegistry *registry, const char *id, UmiTestPlatformBenchmarkSnapshot *out_item);
/**
 * Find test platform benchmark registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_benchmark_registry_at(const UmiTestPlatformBenchmarkRegistry *registry, size_t index, UmiTestPlatformBenchmarkSnapshot *out_item);
/**
 * Return the number of records represented by test platform benchmark registry without
 * changing their state.
 */
size_t umi_test_platform_benchmark_registry_count(const UmiTestPlatformBenchmarkRegistry *registry);
/**
 * Provide the test platform benchmark registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_test_platform_benchmark_registry_revision(const UmiTestPlatformBenchmarkRegistry *registry);
/**
 * Release or reset state held by test platform benchmark registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_benchmark_registry_clear(UmiTestPlatformBenchmarkRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_test_platform_benchmark_snapshot_validate(const UmiTestPlatformBenchmarkSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_TEST_PLATFORM_BENCHMARK_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_test_platform_benchmark_registry_upsert_many(UmiTestPlatformBenchmarkRegistry *registry,
    const UmiTestPlatformBenchmarkSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
