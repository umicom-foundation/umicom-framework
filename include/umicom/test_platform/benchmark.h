/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/benchmark.h
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
#ifndef UMICOM_TEST_PLATFORM_BENCHMARK_H
#define UMICOM_TEST_PLATFORM_BENCHMARK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_BENCHMARK_CAPACITY 4096U
#define UMI_TEST_PLATFORM_BENCHMARK_API_VERSION 1U

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

typedef struct UmiTestPlatformBenchmarkRegistry UmiTestPlatformBenchmarkRegistry;

UmiStatus umi_test_platform_benchmark_registry_create(UmiTestPlatformBenchmarkRegistry **out_registry);
void umi_test_platform_benchmark_registry_destroy(UmiTestPlatformBenchmarkRegistry *registry);
UmiStatus umi_test_platform_benchmark_registry_upsert(UmiTestPlatformBenchmarkRegistry *registry, const UmiTestPlatformBenchmarkSnapshot *item);
UmiStatus umi_test_platform_benchmark_registry_remove(UmiTestPlatformBenchmarkRegistry *registry, const char *id);
UmiStatus umi_test_platform_benchmark_registry_find(const UmiTestPlatformBenchmarkRegistry *registry, const char *id, UmiTestPlatformBenchmarkSnapshot *out_item);
UmiStatus umi_test_platform_benchmark_registry_at(const UmiTestPlatformBenchmarkRegistry *registry, size_t index, UmiTestPlatformBenchmarkSnapshot *out_item);
size_t umi_test_platform_benchmark_registry_count(const UmiTestPlatformBenchmarkRegistry *registry);
uint64_t umi_test_platform_benchmark_registry_revision(const UmiTestPlatformBenchmarkRegistry *registry);
void umi_test_platform_benchmark_registry_clear(UmiTestPlatformBenchmarkRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
