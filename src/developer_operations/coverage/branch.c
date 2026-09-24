/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/coverage/branch.c
 *
 * PURPOSE:
 *   Evaluate Coverage Branch presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/coverage/branch.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateCoverageBranch(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "coverage.branch",
        "Coverage Branch",
        UMI_DEVELOPER_OPERATION_RATIO,
        75.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
