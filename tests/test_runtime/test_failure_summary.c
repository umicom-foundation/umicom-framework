/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_failure_summary.c
 *
 * PURPOSE:
 *   Verify the failure summary runtime contract.
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
#include "umicom/test_runtime/failure_summary.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeFailureSummary value,same;
    uint64_t r;
    umi_test_runtime_failure_summary_init(&value,"test-runtime.failure_summary");
    assert(umi_test_runtime_failure_summary_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_failure_summary_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_summary_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_summary_set_failure_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_summary_set_category_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_summary_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.failure_count==13U&&value.category_count==21U);
    umi_test_runtime_failure_summary_init(&same,"test-runtime.failure_summary");
    assert(umi_test_runtime_failure_summary_same_identity(&value,&same));
    return 0;
    }
