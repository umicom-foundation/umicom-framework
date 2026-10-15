/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/terminal/output_filter.c
 *
 * PURPOSE:
 *   Evaluate Output Filter presentation state without replacing its canonical
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
#include "umicom/developer_operations/terminal/output_filter.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateTerminalOutputFilter(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "terminal.output-filter",
        "Output Filter",
        UMI_DEVELOPER_OPERATION_COUNT,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperOperationsEvaluateTerminalOutputFilterBatch, UmiDeveloperOperationsEvaluateTerminalOutputFilter,
    UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot)
