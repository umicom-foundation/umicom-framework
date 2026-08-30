/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_shard.c
 *
 * PURPOSE:
 *   Verify the shard contract, bounded text and revision behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/shard.h"

int main(void)
{
    UmiTestRuntimeShard value;
    UmiTestRuntimeShard same;
    uint64_t revision;
    umi_test_runtime_shard_init(&value, "test-runtime.shard");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_shard_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_shard_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_shard_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_shard_set_index(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_shard_set_count(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_shard_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.index == 7U);
    assert(value.count == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_shard_init(&same, "test-runtime.shard");
    assert(umi_test_runtime_shard_same_identity(&value, &same));
    assert(umi_test_runtime_shard_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
