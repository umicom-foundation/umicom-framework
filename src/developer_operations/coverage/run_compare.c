/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/coverage/run_compare.c
 *
 * PURPOSE:
 *   Evaluate Coverage Run Compare presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/coverage/run_compare.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateCoverageRunCompare(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "coverage.run-compare",
        "Coverage Run Compare",
        UMI_DEVELOPER_OPERATION_DELTA,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
