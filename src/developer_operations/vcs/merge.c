/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/vcs/merge.c
 *
 * PURPOSE:
 *   Evaluate VCS Merge presentation state without replacing its canonical
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
#include "umicom/developer_operations/vcs/merge.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateVcsMerge(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "vcs.merge",
        "VCS Merge",
        UMI_DEVELOPER_OPERATION_CONFLICT,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperOperationsEvaluateVcsMergeBatch, UmiDeveloperOperationsEvaluateVcsMerge,
    UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot)
