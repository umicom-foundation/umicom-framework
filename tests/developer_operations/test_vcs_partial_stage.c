/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_operations/test_vcs_partial_stage.c
 *
 * PURPOSE:
 *   Verify the VCS Partial Stage operational projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_operations/vcs/partial_stage.h"

int main(void)
{
    UmiDeveloperOperationInput input;
    UmiDeveloperOperationSnapshot snapshot;

    UmiDeveloperOperationInputInit(&input);
    input.primary = 80U;
    input.secondary = 2U;
    input.total = 100U;
    input.baseline = 70.0;
    input.threshold = 0.0;
    input.active = 1;
    input.blocked = 0;
    input.revision = 7U;

    assert(UmiDeveloperOperationsEvaluateVcsPartialStage(&input, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.id, "vcs.partial-stage") == 0);
    assert(snapshot.revision == 7U);
    assert(snapshot.primary == 80U);
    assert(snapshot.total == 100U);
    assert(snapshot.percent > 79.9 && snapshot.percent < 80.1);
    assert(snapshot.ready == 0);
    assert(snapshot.attention == 0);
    return 0;
}
