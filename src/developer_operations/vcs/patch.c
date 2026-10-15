/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/vcs/patch.c
 *
 * PURPOSE:
 *   Evaluate VCS Patch presentation state without replacing its canonical
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
#include "umicom/developer_operations/vcs/patch.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateVcsPatch(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "vcs.patch",
        "VCS Patch",
        UMI_DEVELOPER_OPERATION_PROGRESS,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperOperationsEvaluateVcsPatchBatch, UmiDeveloperOperationsEvaluateVcsPatch,
    UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot)
