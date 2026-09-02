/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/selection_state.h
 *
 * PURPOSE:
 *   Retain the current Studio cross-domain selection independently from toolkit
 *   row-selection widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SELECTION_STATE_H
#define UMICOM_STUDIO_RUNTIME_SELECTION_STATE_H

#include "umicom/studio_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the studio runtime selection state data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeSelectionState {
    UmiStudioRuntimeSelection current;
    uint64_t revision;
} UmiStudioRuntimeSelectionState;

/**
 * Initialise studio selection state from caller-provided values so later operations
 * receive a known state.
 */
void umi_studio_selection_state_init(
    UmiStudioRuntimeSelectionState *state);

/**
 * Copy studio selection state into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_studio_selection_state_set(
    UmiStudioRuntimeSelectionState *state,
    const UmiStudioRuntimeSelection *selection);

/**
 * Release or reset state held by studio selection state so the same storage can be reused
 * safely.
 */
void umi_studio_selection_state_clear(
    UmiStudioRuntimeSelectionState *state);

#ifdef __cplusplus
}
#endif
#endif
