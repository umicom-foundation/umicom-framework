/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/terminal/task_cancellation.c
 *
 * PURPOSE:
 *   Evaluate Task Cancellation presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/terminal/task_cancellation.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateTerminalTaskCancellation(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "terminal.task-cancellation",
        "Task Cancellation",
        UMI_DEVELOPER_OPERATION_ALERT,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
