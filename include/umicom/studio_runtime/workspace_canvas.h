/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/workspace_canvas.h
 *
 * PURPOSE:
 *   Connect Studio's real tool surfaces to the shared editable workspace.
 *   This bridge borrows an existing model; it never owns a second layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_WORKSPACE_CANVAS_H
#define UMICOM_STUDIO_RUNTIME_WORKSPACE_CANVAS_H

#include "umicom/application/suite_layout/geometry.h"
#include "umicom/application_shell/state.h"
#include "umicom/studio_runtime/surface_catalogue.h"
#include "umicom/ui/workspace_customisation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX "umicom.studio.layout."

/* Stable singleton identity shared by the catalogue, layout and widget factory.
 * Returned text belongs to Framework. An unknown kind returns NULL. */
const char *umi_studio_runtime_workspace_surface_id(
    UmiStudioRuntimeSurfaceKind kind);
/* Accept a singleton identity or its existing IDE surface ID. Unknown text
 * leaves the caller's output unchanged. Component IDs are deliberately not
 * accepted: several different tools can use the same component. */
UmiStatus umi_studio_runtime_workspace_surface_kind(
    const char *surface_id, UmiStudioRuntimeSurfaceKind *out_kind);

/* Seed an empty, initialized model from the native surface/preset catalogues.
 * All supported tools are registered, including initially hidden tools. The
 * complete candidate is built on the heap and published only on success.
 * Existing layouts are never silently replaced. All calls are same-thread;
 * the caller keeps this model alive and owns begin/commit/cancel editing. */
UmiStatus umi_studio_runtime_workspace_seed(
    UmiUiWorkspaceCustomisation *model);
/* Select an existing exact layout ID, or a short preset suffix such as default.
 * Selection during an edit is rejected by the existing workspace transaction. */
UmiStatus umi_studio_runtime_workspace_select(
    UmiUiWorkspaceCustomisation *model, const char *layout_id);
/* A new named canvas has no panels and starts an edit session. IDs must use
 * UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX followed by a nonempty name. */
UmiStatus umi_studio_runtime_workspace_create_blank(
    UmiUiWorkspaceCustomisation *model,
    const char *layout_id, const char *name);
/* Add and select a new, locked copy of the current Framework product default.
 * This does not copy a previously customised layout with the default ID, and
 * never replaces existing layouts, documents or saved checkpoints. The caller
 * supplies a unique qualified ID and name. Registered tools are reused; the
 * shared context routing store is preserved exactly and its current mappings
 * are projected into the new layout, including tools with no context group.
 * Missing tool registrations, an active edit or capacity failure leave the
 * complete model unchanged. Saving this new arrangement is a separate action. */
UmiStatus umi_studio_runtime_workspace_create_default(
    UmiUiWorkspaceCustomisation *model,
    const char *layout_id, const char *name);

/* Open/reopen or hide a real native tool without duplicating its singleton.
 * Opening outside Edit Layout makes one short, atomic edit and restores locking.
 * Inside Edit Layout it participates in the caller's existing undo baseline.
 * Hiding requires an active, unlocked edit and an unpinned, closable panel.
 * Hiding the editor hides its panel, not its documents or unsaved buffers. */
UmiStatus umi_studio_runtime_workspace_set_visible(
    UmiUiWorkspaceCustomisation *model,
    UmiStudioRuntimeSurfaceKind kind, bool visible);
/* Normal tool Close/Reopen and Dock/Auto Hide use the same model authority
 * without requiring Edit Layout. A missing real tool is opened through the
 * native catalogue. Auto-hide stores visible=false plus its existing edge
 * prefix; revealing/collapsing that rail is transient adapter presentation.
 * Product capabilities and protected pinning remain enforced. */
UmiStatus umi_studio_runtime_workspace_set_tool_presentation(
    UmiUiWorkspaceCustomisation *model,
    UmiStudioRuntimeSurfaceKind kind, bool visible, bool auto_hidden);
/* Placement remains an explicit edit operation. Canonical product capability
 * flags are respected wherever a native surface has a matching product panel. */
UmiStatus umi_studio_runtime_workspace_apply_panel_settings(
    UmiUiWorkspaceCustomisation *model,
    const UmiUiWorkspacePanelSettings *settings);
/* Reject delayed drag requests when the active layout revision has changed.
 * Success changes only shared model geometry, never GTK widgets or documents. */
UmiStatus umi_studio_runtime_workspace_place_canvas(
    UmiUiWorkspaceCustomisation *model, const char *window_id,
    const UmiApplicationSuiteLayoutRect *rect,
    uint64_t expected_layout_revision);
/* These frame requests join the existing edit session. Pinning prevents later
 * movement. Maximising records a request without replacing saved geometry;
 * an adapter must implement that presentation mode before offering its action. */
UmiStatus umi_studio_runtime_workspace_set_pinned(
    UmiUiWorkspaceCustomisation *model, const char *window_id, bool pinned);
UmiStatus umi_studio_runtime_workspace_set_maximised(
    UmiUiWorkspaceCustomisation *model, const char *window_id, bool maximised);
/* One-way compatibility projection for existing command and status services.
 * The native model stays authoritative and unchanged. The legacy shell cannot
 * store free geometry, so a canvas item projects to its semantic editor region.
 * Call after an accepted model change, never copy this projection back into
 * the native model. Registry bindings must already be installed; no widgets or
 * services are created here. All borrowed objects use the same calling thread. */
UmiStatus umi_studio_runtime_workspace_project_shell(
    const UmiUiWorkspaceCustomisation *model,
    UmiApplicationShellRegistry *registry,
    UmiApplicationShellState *shell_state,
    UmiApplicationShellLayout *shell_layout);

#ifdef __cplusplus
}
#endif
#endif
