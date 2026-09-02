/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_execution_snapshot.c
 *
 * PURPOSE:
 *   Verify the execution snapshot runtime contract.
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
#include "umicom/test_runtime/execution_snapshot.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeExecutionSnapshot value,same;
    uint64_t r;
    umi_test_runtime_execution_snapshot_init(&value,"test-runtime.execution_snapshot");
    assert(umi_test_runtime_execution_snapshot_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_execution_snapshot_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_snapshot_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_snapshot_set_evidence_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_snapshot_set_generation(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_snapshot_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.evidence_count==13U&&value.generation==21U);
    umi_test_runtime_execution_snapshot_init(&same,"test-runtime.execution_snapshot");
    assert(umi_test_runtime_execution_snapshot_same_identity(&value,&same));
    return 0;
    }
