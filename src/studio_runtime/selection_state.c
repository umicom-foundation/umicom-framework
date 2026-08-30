/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/selection_state.c
 *
 * PURPOSE:
 *   Implement bounded cross-domain Studio selection state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/selection_state.h"

#include <string.h>

void umi_studio_selection_state_init(
    UmiStudioRuntimeSelectionState *state)
{
    if (state == NULL) return;
    (void)memset(state, 0, sizeof(*state));
    state->current.kind = UMI_STUDIO_SELECTION_NONE;
    state->revision = 1U;
}

UmiStatus umi_studio_selection_state_set(
    UmiStudioRuntimeSelectionState *state,
    const UmiStudioRuntimeSelection *selection)
{
    if (state == NULL || selection == NULL ||
        selection->kind <= UMI_STUDIO_SELECTION_NONE ||
        selection->kind > UMI_STUDIO_SELECTION_AI_PATCH_FILE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    state->current = *selection;
    state->current.revision = state->revision + 1U;
    state->revision += 1U;
    return UMI_STATUS_OK;
}

void umi_studio_selection_state_clear(
    UmiStudioRuntimeSelectionState *state)
{
    if (state == NULL) return;
    (void)memset(&state->current, 0, sizeof(state->current));
    state->current.kind = UMI_STUDIO_SELECTION_NONE;
    state->revision += 1U;
}
