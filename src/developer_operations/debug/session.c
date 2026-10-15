/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/debug/session.c
 *
 * PURPOSE:
 *   Evaluate Debug Session presentation state without replacing its canonical
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
#include "umicom/developer_operations/debug/session.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateDebugSession(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "debug.session",
        "Debug Session",
        UMI_DEVELOPER_OPERATION_ACTIVITY,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperOperationsEvaluateDebugSessionBatch, UmiDeveloperOperationsEvaluateDebugSession,
    UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot)
