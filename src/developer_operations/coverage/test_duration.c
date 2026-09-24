/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/coverage/test_duration.c
 *
 * PURPOSE:
 *   Evaluate Test Duration presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/coverage/test_duration.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateCoverageTestDuration(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "coverage.test-duration",
        "Test Duration",
        UMI_DEVELOPER_OPERATION_CAPACITY,
        85.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
