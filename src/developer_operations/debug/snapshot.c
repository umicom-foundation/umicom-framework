/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/debug/snapshot.c
 *
 * PURPOSE:
 *   Evaluate Debug Snapshot presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/debug/snapshot.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateDebugSnapshot(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "debug.snapshot",
        "Debug Snapshot",
        UMI_DEVELOPER_OPERATION_PROGRESS,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
