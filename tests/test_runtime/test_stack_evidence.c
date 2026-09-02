/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_stack_evidence.c
 *
 * PURPOSE:
 *   Verify the stack evidence runtime contract.
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
#include "umicom/test_runtime/stack_evidence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeStackEvidence value,same;
    uint64_t r;
    umi_test_runtime_stack_evidence_init(&value,"test-runtime.stack_evidence");
    assert(umi_test_runtime_stack_evidence_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_stack_evidence_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_stack_evidence_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_stack_evidence_set_estimated_bytes(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_stack_evidence_set_depth(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_stack_evidence_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.estimated_bytes==13U&&value.depth==21U);
    umi_test_runtime_stack_evidence_init(&same,"test-runtime.stack_evidence");
    assert(umi_test_runtime_stack_evidence_same_identity(&value,&same));
    return 0;
    }
