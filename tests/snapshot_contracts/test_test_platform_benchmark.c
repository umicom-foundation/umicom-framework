/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_test_platform_benchmark.c
 * PURPOSE: Check test_platform benchmark input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/benchmark.h"
#include "umicom/test_platform/benchmark.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiTestPlatformBenchmarkSnapshot
#define CONTRACT_REGISTRY UmiTestPlatformBenchmarkRegistry
#define CONTRACT_CAPACITY UMI_TEST_PLATFORM_BENCHMARK_CAPACITY
#define CONTRACT_VALIDATE umi_test_platform_benchmark_snapshot_validate
#define CONTRACT_BATCH umi_test_platform_benchmark_registry_upsert_many
#define CONTRACT_CREATE umi_test_platform_benchmark_registry_create
#define CONTRACT_DESTROY umi_test_platform_benchmark_registry_destroy
#define CONTRACT_UPSERT umi_test_platform_benchmark_registry_upsert
#define CONTRACT_REMOVE umi_test_platform_benchmark_registry_remove
#define CONTRACT_FIND umi_test_platform_benchmark_registry_find
#define CONTRACT_AT umi_test_platform_benchmark_registry_at
#define CONTRACT_COUNT umi_test_platform_benchmark_registry_count
#define CONTRACT_REVISION umi_test_platform_benchmark_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiTestPlatformBenchmarkSnapshot, id), sizeof(((UmiTestPlatformBenchmarkSnapshot *)0)->id), 1 },
    {"result_id", offsetof(UmiTestPlatformBenchmarkSnapshot, result_id), sizeof(((UmiTestPlatformBenchmarkSnapshot *)0)->result_id), 0 },
    {"metric", offsetof(UmiTestPlatformBenchmarkSnapshot, metric), sizeof(((UmiTestPlatformBenchmarkSnapshot *)0)->metric), 0 },
    {"unit", offsetof(UmiTestPlatformBenchmarkSnapshot, unit), sizeof(((UmiTestPlatformBenchmarkSnapshot *)0)->unit), 0 }
};
static int ContractSnapshotEqual(const UmiTestPlatformBenchmarkSnapshot *left, const UmiTestPlatformBenchmarkSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->result_id, right->result_id, sizeof(left->result_id)) == 0 &&
        memcmp(left->metric, right->metric, sizeof(left->metric)) == 0 &&
        memcmp(left->unit, right->unit, sizeof(left->unit)) == 0 &&
        left->value == right->value &&
        left->baseline == right->baseline &&
        left->tolerance == right->tolerance &&
        left->regression == right->regression &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiTestPlatformBenchmarkSnapshot *item)
{
    item->result_id[0] = 'v';
    item->metric[0] = 'v';
    item->unit[0] = 'v';
    item->value = (double)7U;
    item->baseline = (double)8U;
    item->tolerance = (double)9U;
    item->regression = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_test_platform_benchmark_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_test_platform_benchmark_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiTestPlatformBenchmarkEdit
#define CONTRACT_EDIT_CURRENT umi_test_platform_benchmark_registry_edit_if_current
#include "snapshot_contract_cases.h"
