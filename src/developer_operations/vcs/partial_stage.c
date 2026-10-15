/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/vcs/partial_stage.c
 *
 * PURPOSE:
 *   Evaluate VCS Partial Stage presentation state without replacing its canonical
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
#include "umicom/developer_operations/vcs/partial_stage.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateVcsPartialStage(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "vcs.partial-stage",
        "VCS Partial Stage",
        UMI_DEVELOPER_OPERATION_PROGRESS,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperOperationsEvaluateVcsPartialStageBatch, UmiDeveloperOperationsEvaluateVcsPartialStage,
    UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot)
