/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/debug/call_stack.c
 *
 * PURPOSE:
 *   Evaluate Debug Call Stack presentation state without replacing its canonical
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
#include "umicom/developer_operations/debug/call_stack.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateDebugCallStack(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "debug.call-stack",
        "Debug Call Stack",
        UMI_DEVELOPER_OPERATION_COUNT,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperOperationsEvaluateDebugCallStackBatch, UmiDeveloperOperationsEvaluateDebugCallStack,
    UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot)
