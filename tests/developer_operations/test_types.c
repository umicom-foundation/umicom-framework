/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_operations/test_types.c
 *
 * PURPOSE:
 *   Verify developer-operation snapshot validation and initialisation.
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

#include "umicom/developer_operations/types.h"

int main(void)
{
    UmiDeveloperOperationInput input;
    UmiDeveloperOperationSnapshot snapshot;

    UmiDeveloperOperationInputInit(&input);
    UmiDeveloperOperationSnapshotInit(&snapshot);
    assert(input.primary == 0U);
    assert(snapshot.id[0] == '\0');

    (void)strcpy(snapshot.id, "coverage.file");
    snapshot.percent = 84.0;
    snapshot.ready = 1;
    snapshot.blocked = 0;
    assert(UmiDeveloperOperationSnapshotValidate(&snapshot) == UMI_STATUS_OK);

    snapshot.percent = 101.0;
    assert(UmiDeveloperOperationSnapshotValidate(&snapshot) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
