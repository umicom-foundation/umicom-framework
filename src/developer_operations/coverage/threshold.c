/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/coverage/threshold.c
 *
 * PURPOSE:
 *   Evaluate Coverage Threshold presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/coverage/threshold.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateCoverageThreshold(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "coverage.threshold",
        "Coverage Threshold",
        UMI_DEVELOPER_OPERATION_THRESHOLD,
        85.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
