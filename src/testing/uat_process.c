/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/uat_process.c
 *
 * PURPOSE:
 *   Connect UAT workflow steps to the existing portable test-process service
 *   rather than introducing a second process execution implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/uat_workflow.h"
#include "umicom/testing/runner.h"
UmiStatus UmiUatExecuteProcess(void *context, const UmiTestCase *test,
    UmiCancellationToken *cancellation, UmiTestResult *result)
{
    (void)context;
    return umi_test_runner_run_case(test, cancellation, result);
}
