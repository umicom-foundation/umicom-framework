/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/coverage/test_result_summary.c
 *
 * PURPOSE:
 *   Evaluate Test Result Summary presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/coverage/test_result_summary.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateCoverageTestResultSummary(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "coverage.test-result-summary",
        "Test Result Summary",
        UMI_DEVELOPER_OPERATION_PROGRESS,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
