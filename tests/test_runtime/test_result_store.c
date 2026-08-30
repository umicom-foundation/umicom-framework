/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_result_store.c
 *
 * PURPOSE:
 *   Verify the result store contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/result_store.h"

int main(void)
{
    UmiTestRuntimeResultStore value;
    UmiTestRuntimeResultStore same;
    uint64_t revision;
    umi_test_runtime_result_store_init(&value, "test-runtime.result_store");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_result_store_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_result_store_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_result_store_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_result_store_set_result_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_result_store_set_generation(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_result_store_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.result_count == 7U);
    assert(value.generation == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_result_store_init(&same, "test-runtime.result_store");
    assert(umi_test_runtime_result_store_same_identity(&value, &same));
    assert(umi_test_runtime_result_store_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
