/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_intelligence/test_types.c
 *
 * PURPOSE:
 *   Verify developer-intelligence signal initialisation and snapshot validation.
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

#include "umicom/developer_intelligence/types.h"

int main(void)
{
    UmiDeveloperIntelligenceInput input;
    UmiDeveloperIntelligenceSnapshot snapshot;

    UmiDeveloperIntelligenceInputInit(&input);
    assert(input.remoteAllowed == 1);

    UmiDeveloperIntelligenceSnapshotInit(&snapshot);
    (void)strcpy(snapshot.id, "language.completion-rank");
    snapshot.score = 72.0;
    snapshot.confidence = 80.0;
    snapshot.risk = 10.0;
    snapshot.ready = 1;
    assert(UmiDeveloperIntelligenceSnapshotValidate(&snapshot) == UMI_STATUS_OK);

    snapshot.score = 110.0;
    assert(UmiDeveloperIntelligenceSnapshotValidate(&snapshot) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
