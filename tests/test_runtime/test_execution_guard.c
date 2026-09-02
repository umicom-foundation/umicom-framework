/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_execution_guard.c
 *
 * PURPOSE:
 *   Verify the execution guard runtime contract.
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
#include "umicom/test_runtime/execution_guard.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeExecutionGuard value,same;
    uint64_t r;
    umi_test_runtime_execution_guard_init(&value,"test-runtime.execution_guard");
    assert(umi_test_runtime_execution_guard_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_execution_guard_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_guard_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_guard_set_guard_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_guard_set_rejected_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_guard_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.guard_count==13U&&value.rejected_count==21U);
    umi_test_runtime_execution_guard_init(&same,"test-runtime.execution_guard");
    assert(umi_test_runtime_execution_guard_same_identity(&value,&same));
    return 0;
    }
