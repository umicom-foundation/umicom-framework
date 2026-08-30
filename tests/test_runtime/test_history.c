/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_history.c
 *
 * PURPOSE:
 *   Verify the history contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/history.h"

int main(void)
{
    UmiTestRuntimeHistory value;
    UmiTestRuntimeHistory same;
    uint64_t revision;
    umi_test_runtime_history_init(&value, "test-runtime.history");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_history_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_history_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_history_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_history_set_run_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_history_set_generation(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_history_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.run_count == 7U);
    assert(value.generation == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_history_init(&same, "test-runtime.history");
    assert(umi_test_runtime_history_same_identity(&value, &same));
    assert(umi_test_runtime_history_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
