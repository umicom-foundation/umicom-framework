/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/debug/toolbar.c
 *
 * PURPOSE:
 *   Evaluate Debug Toolbar presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/debug/toolbar.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateDebugToolbar(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "debug.toolbar",
        "Debug Toolbar",
        UMI_DEVELOPER_OPERATION_ACTIVITY,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
