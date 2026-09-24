/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/debug/stop_reason.c
 *
 * PURPOSE:
 *   Evaluate Debug Stop Reason presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/debug/stop_reason.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateDebugStopReason(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "debug.stop-reason",
        "Debug Stop Reason",
        UMI_DEVELOPER_OPERATION_ALERT,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
