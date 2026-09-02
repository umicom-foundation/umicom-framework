/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_execution_batch.c
 *
 * PURPOSE:
 *   Verify the execution batch contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/execution_batch.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeExecutionBatch value;
    UmiTestRuntimeExecutionBatch same;
    uint64_t revision;
    umi_test_runtime_execution_batch_init(&value, "test-runtime.execution_batch");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_execution_batch_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_execution_batch_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_execution_batch_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_execution_batch_set_planned_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_execution_batch_set_completed_count(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_execution_batch_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.planned_count == 7U);
    assert(value.completed_count == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_execution_batch_init(&same, "test-runtime.execution_batch");
    assert(umi_test_runtime_execution_batch_same_identity(&value, &same));
    assert(umi_test_runtime_execution_batch_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
