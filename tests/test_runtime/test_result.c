/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_result.c
 *
 * PURPOSE:
 *   Verify the result contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/result.h"

int main(void)
{
    UmiTestRuntimeResult value;
    UmiTestRuntimeResult same;
    uint64_t revision;
    umi_test_runtime_result_init(&value, "test-runtime.result");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_result_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_result_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_result_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_result_set_duration_ms(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_result_set_exit_code(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_result_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.duration_ms == 7U);
    assert(value.exit_code == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_result_init(&same, "test-runtime.result");
    assert(umi_test_runtime_result_same_identity(&value, &same));
    assert(umi_test_runtime_result_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
