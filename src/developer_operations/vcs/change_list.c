/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/vcs/change_list.c
 *
 * PURPOSE:
 *   Evaluate VCS Change List presentation state without replacing its canonical
 *   service or retaining caller-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/vcs/change_list.h"
#include "../developer_operations_internal.h"

UmiStatus UmiDeveloperOperationsEvaluateVcsChangeList(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    static const UmiDeveloperOperationPolicy policy = {
        "vcs.change-list",
        "VCS Change List",
        UMI_DEVELOPER_OPERATION_CLEANLINESS,
        0.0
    };
    return UmiDeveloperOperationsEvaluatePolicy(&policy, input, outSnapshot);
}
