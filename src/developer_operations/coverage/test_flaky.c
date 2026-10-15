/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/coverage/test_flaky.c
 *
 * PURPOSE:
 *   Evaluate Flaky Test presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/developer_operations/coverage/test_flaky.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateCoverageTestFlaky(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "coverage.test-flaky",
        "Flaky Test",
        UMI_DEVELOPER_OPERATION_ALERT,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperOperationsEvaluateCoverageTestFlakyBatch, UmiDeveloperOperationsEvaluateCoverageTestFlaky,
    UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot)
