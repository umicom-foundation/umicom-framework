/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_run_summary.c
 *
 * PURPOSE:
 *   Verify the run summary runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/run_summary.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeRunSummary v,s;
    umi_test_runtime_run_summary_init(&v,"test-runtime.run_summary");
    assert(umi_test_runtime_run_summary_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_run_summary_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_run_summary_set_passed_count(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_run_summary_set_failed_count(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_run_summary_init(&s,"test-runtime.run_summary");
    assert(umi_test_runtime_run_summary_same_identity(&v,&s));
    return 0;
    }
