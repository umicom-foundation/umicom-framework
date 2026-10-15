/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/terminal/history.c
 *
 * PURPOSE:
 *   Evaluate Terminal History presentation state without replacing its canonical
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
#include "umicom/developer_operations/terminal/history.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateTerminalHistory(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "terminal.history",
        "Terminal History",
        UMI_DEVELOPER_OPERATION_CAPACITY,
        90.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperOperationsEvaluateTerminalHistoryBatch, UmiDeveloperOperationsEvaluateTerminalHistory,
    UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot)
