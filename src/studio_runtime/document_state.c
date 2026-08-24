/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/document_state.c
 *
 * PURPOSE:
 *   Implement DocumentCoordinator-to-Studio presentation projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/document_state.h"

#include <string.h>

void umi_studio_document_state_init(
    UmiStudioRuntimeDocumentState *state)
{
    if (state == NULL) return;
    (void)memset(state, 0, sizeof(*state));
    state->revision = 1U;
}

UmiStatus umi_studio_document_state_refresh(
    UmiStudioRuntimeDocumentState *state,
    UmiDocumentCoordinator *documents)
{
    int changed = 0;
    UmiStatus status;

    if (state == NULL || documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    state->open_document_count =
        umi_document_coordinator_count(documents);

    status = umi_document_coordinator_active_snapshot(
        documents,
        &state->active);

    if (status == UMI_STATUS_NOT_FOUND) {
        (void)memset(&state->active, 0, sizeof(state->active));
        state->has_active = 0;
        state->external_change_detected = 0;
        state->revision += 1U;
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) return status;

    state->has_active = 1;

    status = umi_document_coordinator_check_external_change(
        documents,
        &changed);
    if (status != UMI_STATUS_OK) return status;

    state->external_change_detected = changed;
    state->revision += 1U;
    return UMI_STATUS_OK;
}
