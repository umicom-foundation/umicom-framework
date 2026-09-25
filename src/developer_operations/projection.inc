/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/projection.inc
 *
 * PURPOSE:
 *   Bridge distinct public projection names to the retained compact catalogue without pointer casts or ABI substitution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/projection.h"
#include "umicom/developer_operations/service.h"
#include <math.h>

void UmiDeveloperOperationProjectionInputInit(UmiDeveloperOperationProjectionInput *input)
{
    if (input != NULL) memset(input, 0, sizeof(*input));
}

UmiStatus UmiDeveloperOperationsProjectionEvaluate(const char *id,
    const UmiDeveloperOperationProjectionInput *input,
    UmiDeveloperOperationProjectionSnapshot *outSnapshot)
{
    UmiDeveloperOperationInput legacyInput;
    UmiDeveloperOperationSnapshot legacySnapshot;
    UmiStatus status;
    if (outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outSnapshot, 0, sizeof(*outSnapshot));
    if (input == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    UmiDeveloperOperationInputInit(&legacyInput);
    UmiDeveloperOperationSnapshotInit(&legacySnapshot);
    legacyInput.label = input->label;
    legacyInput.primary = input->primary;
    legacyInput.secondary = input->secondary;
    legacyInput.total = input->total;
    legacyInput.baseline = input->baseline;
    legacyInput.threshold = input->threshold;
    legacyInput.active = input->active;
    legacyInput.blocked = input->blocked;
    legacyInput.revision = input->revision;
    status = UmiDeveloperOperationsServiceEvaluate(id, &legacyInput, &legacySnapshot);
    if (status != UMI_STATUS_OK) return status;
    status = UmiDeveloperOperationSnapshotValidate(&legacySnapshot);
    if (status != UMI_STATUS_OK) return status;
    if (memchr(legacySnapshot.id, '\0', sizeof(legacySnapshot.id)) == NULL ||
        memchr(legacySnapshot.label, '\0', sizeof(legacySnapshot.label)) == NULL ||
        !isfinite(legacySnapshot.percent) || !isfinite(legacySnapshot.delta)) return UMI_STATUS_INVALID_STATE;
    _Static_assert(sizeof(outSnapshot->id) == sizeof(legacySnapshot.id), "Projection id capacity changed");
    _Static_assert(sizeof(outSnapshot->label) == sizeof(legacySnapshot.label), "Projection label capacity changed");
    memcpy(outSnapshot->id, legacySnapshot.id, sizeof(outSnapshot->id));
    memcpy(outSnapshot->label, legacySnapshot.label, sizeof(outSnapshot->label));
    outSnapshot->primary = legacySnapshot.primary;
    outSnapshot->secondary = legacySnapshot.secondary;
    outSnapshot->total = legacySnapshot.total;
    outSnapshot->missed = legacySnapshot.missed;
    outSnapshot->percent = legacySnapshot.percent;
    outSnapshot->delta = legacySnapshot.delta;
    outSnapshot->active = legacySnapshot.active;
    outSnapshot->blocked = legacySnapshot.blocked;
    outSnapshot->ready = legacySnapshot.ready;
    outSnapshot->attention = legacySnapshot.attention;
    outSnapshot->revision = legacySnapshot.revision;
    return UMI_STATUS_OK;
}
