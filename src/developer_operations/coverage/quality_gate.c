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
#include "../../base/policy_batch_internal.h"
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

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperOperationsEvaluateCoverageQualityGateBatch, UmiDeveloperOperationsEvaluateCoverageQualityGate,
    UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot)
