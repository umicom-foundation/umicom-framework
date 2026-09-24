/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/types.c
 *
 * PURPOSE:
 *   Initialise and validate shared developer-intelligence workbench state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/types.h"

#include <string.h>

void UmiDeveloperIntelligenceInputInit(UmiDeveloperIntelligenceInput *input)
{
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
    input->remoteAllowed = 1;
}

void UmiDeveloperIntelligenceSnapshotInit(UmiDeveloperIntelligenceSnapshot *snapshot)
{
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
}

UmiStatus UmiDeveloperIntelligenceSnapshotValidate(
    const UmiDeveloperIntelligenceSnapshot *snapshot)
{
    if (snapshot == NULL || snapshot->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (snapshot->score < 0.0 || snapshot->score > 100.0 ||
        snapshot->confidence < 0.0 || snapshot->confidence > 100.0 ||
        snapshot->risk < 0.0 || snapshot->risk > 100.0) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (snapshot->ready && snapshot->blocked) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (snapshot->approvalRequired && snapshot->ready) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
