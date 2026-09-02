/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/document_state.c
 *
 * PURPOSE:
 *   Implement DocumentCoordinator-to-Studio presentation projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/document_state.h"

#include <string.h>

/*
 * Initialise studio document state from caller-provided values so later operations receive
 * a known state.
 */
void umi_studio_document_state_init(
    UmiStudioRuntimeDocumentState *state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return;
    (void)memset(state, 0, sizeof(*state));
    state->revision = 1U;
}

/*
 * Provide the studio document state refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_document_state_refresh(
    UmiStudioRuntimeDocumentState *state,
    UmiDocumentCoordinator *documents)
{
    int changed = 0;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    state->open_document_count =
        umi_document_coordinator_count(documents);

    status = umi_document_coordinator_active_snapshot(
        documents,
        &state->active);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) {
        (void)memset(&state->active, 0, sizeof(state->active));
        state->has_active = 0;
        state->external_change_detected = 0;
        state->revision += 1U;
        return UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    state->has_active = 1;

    status = umi_document_coordinator_check_external_change(
        documents,
        &changed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    state->external_change_detected = changed;
    state->revision += 1U;
    return UMI_STATUS_OK;
}
