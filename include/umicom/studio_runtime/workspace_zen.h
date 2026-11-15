/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/workspace_zen.h
 *
 * PURPOSE:
 *   Toggle the existing Zen workspace while retaining a session-local return identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_WORKSPACE_ZEN_H
#define UMICOM_STUDIO_RUNTIME_WORKSPACE_ZEN_H
#include "umicom/studio_runtime/workspace_canvas.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Zero-initialise one state per workspace owner. This remembers an identity,
 * not a second layout, and is deliberately absent from saved checkpoints.
 * Keep state and model in separate storage on the same calling thread. */
typedef struct UmiStudioWorkspaceZenState {
    char return_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
} UmiStudioWorkspaceZenState;

/* Read-only admission: no allocation, registry mutation or presentation.
 * A missing previous layout falls back to the existing default. A missing
 * Zen/default target, malformed identity, edit or exhausted revision refuses
 * the toggle. A restored/directly selected Zen layout also exits to default. */
bool umi_studio_runtime_workspace_zen_can_toggle(
    const UmiUiWorkspaceCustomisation *model,
    const UmiStudioWorkspaceZenState *state);

/* Select only an existing layout and update return state on success.
 * Layout contents, groups, library, drafts and checkpoints are unchanged.
 * Failure preserves both objects. Native clients stage copies and publish
 * them only after their existing renderer accepts the candidate model. */
UmiStatus umi_studio_runtime_workspace_toggle_zen(
    UmiUiWorkspaceCustomisation *model, UmiStudioWorkspaceZenState *state);
#ifdef __cplusplus
}
#endif
#endif
