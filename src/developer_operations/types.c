/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/types.c
 *
 * PURPOSE:
 *   Initialise and validate shared developer-operation projection values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/types.h"

#include <string.h>

void UmiDeveloperOperationInputInit(UmiDeveloperOperationInput *input)
{
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
}

void UmiDeveloperOperationSnapshotInit(UmiDeveloperOperationSnapshot *snapshot)
{
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
}

UmiStatus UmiDeveloperOperationSnapshotValidate(
    const UmiDeveloperOperationSnapshot *snapshot)
{
    if (snapshot == NULL || snapshot->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (snapshot->percent < 0.0 || snapshot->percent > 100.0) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (snapshot->ready && snapshot->blocked) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* The compatibility facade is compiled with the existing compact type owner.
 * This keeps its old public symbols and complete implementation intact while
 * allowing Studio to include both developer service families. The facade only
 * delegates projections; it does not copy Test, Debug, Terminal or VCS engines.
 * Its private include is intentionally compiled once, through this file. */
#include "projection.inc"
