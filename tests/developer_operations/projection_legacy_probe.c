/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_operations/projection_legacy_probe.c
 *
 * PURPOSE:
 *   Provide an independent legacy-catalogue probe for field-by-field projection compatibility tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/service.h"
#include "umicom/developer_operations/projection.h"
#include <string.h>

UmiStatus UmiTestLegacyProjection(const char *id,
    const UmiDeveloperOperationProjectionInput *input,
    UmiDeveloperOperationProjectionSnapshot *out);

UmiStatus UmiTestLegacyProjection(const char *id,
    const UmiDeveloperOperationProjectionInput *input,
    UmiDeveloperOperationProjectionSnapshot *out)
{
    UmiDeveloperOperationInput legacy;
    UmiDeveloperOperationSnapshot result;
    UmiStatus status;
    UmiDeveloperOperationInputInit(&legacy);
    UmiDeveloperOperationSnapshotInit(&result);
    legacy.label = input->label;
    legacy.primary = input->primary; legacy.secondary = input->secondary; legacy.total = input->total;
    legacy.baseline = input->baseline; legacy.threshold = input->threshold;
    legacy.active = input->active; legacy.blocked = input->blocked; legacy.revision = input->revision;
    status = UmiDeveloperOperationsServiceEvaluate(id, &legacy, &result);
    if (status != UMI_STATUS_OK) return status;
    memset(out, 0, sizeof(*out));
    memcpy(out->id, result.id, sizeof(out->id)); memcpy(out->label, result.label, sizeof(out->label));
    out->primary = result.primary; out->secondary = result.secondary; out->total = result.total;
    out->missed = result.missed; out->percent = result.percent; out->delta = result.delta;
    out->active = result.active; out->blocked = result.blocked; out->ready = result.ready;
    out->attention = result.attention; out->revision = result.revision;
    return UMI_STATUS_OK;
}
