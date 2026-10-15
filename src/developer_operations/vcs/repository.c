/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/vcs/repository.c
 *
 * PURPOSE:
 *   Evaluate VCS Repository presentation state without replacing its canonical
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
#include "umicom/developer_operations/vcs/repository.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateVcsRepository(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "vcs.repository",
        "VCS Repository",
        UMI_DEVELOPER_OPERATION_ACTIVITY,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperOperationsEvaluateVcsRepositoryBatch, UmiDeveloperOperationsEvaluateVcsRepository,
    UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot)
