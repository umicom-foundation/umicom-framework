/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_operations/test_terminal_search.c
 *
 * PURPOSE:
 *   Verify the Terminal Search operational projection.
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

#include "umicom/developer_operations/terminal/search.h"

#include "../policy_batch/cases.h"
UMI_TEST_POLICY_BATCH(UmiDeveloperOperationsEvaluateTerminalSearchBatchCases, UmiDeveloperOperationsEvaluateTerminalSearch,
    UmiDeveloperOperationsEvaluateTerminalSearchBatch, UmiDeveloperOperationInput, UmiDeveloperOperationSnapshot, baseline)

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

    if (UmiDeveloperOperationsEvaluateTerminalSearchBatchCases(&input) != 0) return 1;
    assert(UmiDeveloperOperationsEvaluateTerminalSearch(&input, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.id, "terminal.search") == 0);
    assert(snapshot.revision == 7U);
    assert(snapshot.primary == 80U);
    assert(snapshot.total == 100U);
    assert(snapshot.percent > 79.9 && snapshot.percent < 80.1);
    assert(snapshot.ready == 1);
    assert(snapshot.attention == 1);
    return 0;
}
