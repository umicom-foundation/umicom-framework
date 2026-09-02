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

/*
 * Initialise studio selection state from caller-provided values so later operations
 * receive a known state.
 */
void umi_studio_selection_state_init(
    UmiStudioRuntimeSelectionState *state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return;
    (void)memset(state, 0, sizeof(*state));
    state->current.kind = UMI_STUDIO_SELECTION_NONE;
    state->revision = 1U;
}

/*
 * Copy studio selection state into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_studio_selection_state_set(
    UmiStudioRuntimeSelectionState *state,
    const UmiStudioRuntimeSelection *selection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Release or reset state held by studio selection state so the same storage can be reused
 * safely.
 */
void umi_studio_selection_state_clear(
    UmiStudioRuntimeSelectionState *state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return;
    (void)memset(&state->current, 0, sizeof(state->current));
    state->current.kind = UMI_STUDIO_SELECTION_NONE;
    state->revision += 1U;
}
