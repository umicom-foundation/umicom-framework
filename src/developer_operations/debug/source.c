/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/debug/source.c
 *
 * PURPOSE:
 *   Evaluate Debug Source presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/debug/source.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateDebugSource(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "debug.source",
        "Debug Source",
        UMI_DEVELOPER_OPERATION_COUNT,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
