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

typedef struct UmiStudioRuntimeSelectionState {
    UmiStudioRuntimeSelection current;
    uint64_t revision;
} UmiStudioRuntimeSelectionState;

void umi_studio_selection_state_init(
    UmiStudioRuntimeSelectionState *state);

UmiStatus umi_studio_selection_state_set(
    UmiStudioRuntimeSelectionState *state,
    const UmiStudioRuntimeSelection *selection);

void umi_studio_selection_state_clear(
    UmiStudioRuntimeSelectionState *state);

#ifdef __cplusplus
}
#endif
#endif
