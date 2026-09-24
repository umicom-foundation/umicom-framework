/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/coverage/summary.c
 *
 * PURPOSE:
 *   Evaluate Coverage Summary presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/coverage/summary.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateCoverageSummary(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "coverage.summary",
        "Coverage Summary",
        UMI_DEVELOPER_OPERATION_THRESHOLD,
        80.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
