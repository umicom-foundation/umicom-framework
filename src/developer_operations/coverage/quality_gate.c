/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/coverage/quality_gate.c
 *
 * PURPOSE:
 *   Evaluate Quality Gate presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/coverage/quality_gate.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateCoverageQualityGate(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "coverage.quality-gate",
        "Quality Gate",
        UMI_DEVELOPER_OPERATION_THRESHOLD,
        90.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
