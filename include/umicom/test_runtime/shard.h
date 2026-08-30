/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/shard.h
 *
 * PURPOSE:
 *   Describe deterministic sharding for large regression suites.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SHARD
#define UMICOM_TEST_RUNTIME_SHARD

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeShard {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t index;
    uint64_t count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeShard;

void umi_test_runtime_shard_init(UmiTestRuntimeShard *value, const char *id);
UmiStatus umi_test_runtime_shard_validate(const UmiTestRuntimeShard *value);
UmiStatus umi_test_runtime_shard_set_name(UmiTestRuntimeShard *value, const char *name);
UmiStatus umi_test_runtime_shard_set_detail(UmiTestRuntimeShard *value, const char *detail);
UmiStatus umi_test_runtime_shard_set_index(UmiTestRuntimeShard *value, uint64_t number);
UmiStatus umi_test_runtime_shard_set_count(UmiTestRuntimeShard *value, uint64_t number);
UmiStatus umi_test_runtime_shard_touch(UmiTestRuntimeShard *value, uint64_t updated_at_ms);
bool umi_test_runtime_shard_same_identity(const UmiTestRuntimeShard *left, const UmiTestRuntimeShard *right);

#ifdef __cplusplus
}
#endif
#endif
