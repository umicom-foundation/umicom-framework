/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/coverage/file.c
 *
 * PURPOSE:
 *   Evaluate Coverage File presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/coverage/file.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateCoverageFile(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "coverage.file",
        "Coverage File",
        UMI_DEVELOPER_OPERATION_RATIO,
        80.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
