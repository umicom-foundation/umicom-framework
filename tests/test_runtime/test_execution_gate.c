/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_execution_gate.c
 *
 * PURPOSE:
 *   Verify the execution gate runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/execution_gate.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeExecutionGate v,s;
    umi_test_runtime_execution_gate_init(&v,"test-runtime.execution_gate");
    assert(umi_test_runtime_execution_gate_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_gate_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_gate_set_required_count(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_gate_set_blocked_count(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_execution_gate_init(&s,"test-runtime.execution_gate");
    assert(umi_test_runtime_execution_gate_same_identity(&v,&s));
    return 0;
    }
