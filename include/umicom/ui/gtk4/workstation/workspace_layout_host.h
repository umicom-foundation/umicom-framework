/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/workspace_layout_host.h
 *
 * PURPOSE:
 *   Render canonical UmiUiWorkspaceLayout geometry through the existing GTK4
 *   workstation tab, panel, split, docking and free-positioned canvas primitives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_WORKSTATION_WORKSPACE_LAYOUT_HOST_H
#define UMICOM_UI_GTK4_WORKSTATION_WORKSPACE_LAYOUT_HOST_H

#include <stddef.h>
#include <stdint.h>

#include <gtk/gtk.h>

#include "umicom/application/suite_layout/render_plan.h"
#include "umicom/ui/workstation/panel_chrome.h"
#include "umicom/ui/window_group.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef GtkWidget *(*UmiGtk4WorkspaceLayoutPanelFactory)(
    const UmiUiWorkspaceWindow *window,
    void *user_data);
typedef void (*UmiGtk4WorkspaceLayoutActionHandler)(
    const char *window_id,
    UmiWsPanelAction action,
    void *user_data);

/** Receive one completed canvas gesture on the GTK owning thread.
 * The rectangle uses fractions of the complete host viewport. The owner must
 * validate the expected model revision, apply its existing edit transaction and
 * rebuild on success. It may rebuild or destroy the host inside this callback.
 */
typedef UmiStatus (*UmiGtk4WorkspaceCanvasGeometryHandler)(
    const char *window_id,
    const UmiApplicationSuiteLayoutRect *rect,
    uint64_t expected_layout_revision,
    void *user_data);

/**
 * Represent the gtk4 workspace layout host snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiGtk4WorkspaceLayoutHostSnapshot {
    char layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    size_t stack_count;
    size_t panel_count;
    size_t placeholder_count;
    size_t floating_count;
    uint64_t revision;
    size_t canvas_count;
    int canvas_editable;
    int geometry_pending;
    uint64_t source_layout_revision;
    /* Rebuild consumers when the public snapshot grows. Retention is opt-in. */
    size_t retained_content_count;
    int content_retention_enabled;
} UmiGtk4WorkspaceLayoutHostSnapshot;

/**
 * Represent the gtk4 workspace layout host data shared with callers of this public
 * contract.
 */
typedef struct UmiGtk4WorkspaceLayoutHost UmiGtk4WorkspaceLayoutHost;

/**
 * Initialise gtk4 workspace layout host from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_gtk4_workspace_layout_host_create(
    const UmiUiWorkspaceLayout *layout,
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory,
    void *user_data,
    UmiGtk4WorkspaceLayoutHost **out_host);
/**
 * Provide the gtk4 workspace layout host create interactive operation used by this module
 * and its client applications.
 */
UmiStatus umi_gtk4_workspace_layout_host_create_interactive(
    const UmiUiWorkspaceLayout *layout,
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory,
    void *panel_user_data,
    UmiGtk4WorkspaceLayoutActionHandler action_handler,
    void *action_user_data,
    UmiGtk4WorkspaceLayoutHost **out_host);
/**
 * Cancel queued requests, detach native callbacks and release the host on its
 * GTK owning thread. A retained root is emptied; retained old controls cannot
 * dispatch into this owner. May be called from the canvas geometry callback.
 * The host pointer is invalid as soon as this function returns.
 */
void umi_gtk4_workspace_layout_host_destroy(
    UmiGtk4WorkspaceLayoutHost *host);
/**
 * Set the optional native parent used by subsequently created floating panel windows.
 */
void umi_gtk4_workspace_layout_host_set_transient_parent(
    UmiGtk4WorkspaceLayoutHost *host,
    GtkWindow *parent);
/** Preserve provider bodies across moves, lock changes and layout selection.
 * Call on an empty rendered host before its first nonempty rebuild. Changing
 * this policy while bodies or visible panels exist returns BUSY. Default is
 * disabled, preserving ordinary refresh-by-rebuild behaviour. The GTK thread
 * owns the cache, bounded to UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS unique IDs.
 * Bodies are keyed by window ID and tool ID, and stay alive while hidden or
 * absent from the selected layout. Capacity exhaustion rejects a rebuild
 * without replacing the view; explicitly invalidate unused bodies to free
 * room. A factory must return an unparented widget and keep model state outside
 * the widget. This preserves widget drafts, not durable application storage.
 */
UmiStatus umi_gtk4_workspace_layout_host_set_content_retention(
    UmiGtk4WorkspaceLayoutHost *host, int enabled);
/** Request fresh provider content during the next successful full rebuild.
 * A NULL ID invalidates all retained bodies; an unknown ID returns NOT_FOUND.
 * This does not destroy the currently displayed widget immediately. Callers
 * must commit or intentionally discard its draft before requesting refresh.
 * Returns INVALID_STATE if content retention is disabled.
 */
UmiStatus umi_gtk4_workspace_layout_host_invalidate_content(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id);
/** Focus a currently visible window without changing model placement.
 * Selects a dock tab, raises a canvas view, or presents an existing detached
 * window. Hidden/missing windows return NOT_FOUND. Canvas front order here is
 * temporary; a later model rebuild restores its recorded z-order. GTK thread
 * only. This may present an existing native window but never creates one.
 */
UmiStatus umi_gtk4_workspace_layout_host_focus_window(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id);
/** Borrow the existing workspace group store to resolve header colours.
 * The store must outlive the host, and updates happen on the GTK thread.
 * This immediately refreshes current frames and is used on future rebuilds.
 * It never creates a second group registry or changes routing membership.
 * NULL removes the binding; unknown groups receive no colour. Invalid bounded
 * records are rejected without replacing the current binding.
 */
UmiStatus umi_gtk4_workspace_layout_host_set_context_groups(
    UmiGtk4WorkspaceLayoutHost *host, const UmiUiWindowGroupStore *groups);
/** Replace the borrowed canvas geometry handler on the GTK owning thread.
 * Replacing it cancels pending gestures/requests; NULL disables manipulation.
 */
UmiStatus umi_gtk4_workspace_layout_host_set_canvas_geometry_handler(
    UmiGtk4WorkspaceLayoutHost *host,
    UmiGtk4WorkspaceCanvasGeometryHandler handler,
    void *user_data);
/** Queue one completed geometry request, also used by native pointer gestures.
 * OK means queued, not accepted. The owner callback runs once from an idle
 * source after all preview geometry has reverted to the model. Rebuild/destroy
 * cancels outstanding requests. No host access occurs after the callback.
 * Locked/pinned or prohibited resize returns PERMISSION_DENIED; a stale model
 * revision returns INVALID_STATE; another pending request returns BUSY.
 * Missing visible canvas IDs return NOT_FOUND; no handler returns UNAVAILABLE;
 * malformed or out-of-bounds rectangles return INVALID_ARGUMENT. Call on the
 * GTK owning thread. The owner callback's result does not implicitly save or
 * update the view; only its accepted model rebuild changes model-backed geometry.
 */
UmiStatus umi_gtk4_workspace_layout_host_request_canvas_geometry(
    UmiGtk4WorkspaceLayoutHost *host,
    const char *window_id,
    const UmiApplicationSuiteLayoutRect *rect,
    uint64_t expected_layout_revision);
/** Copy the current normalized canvas view rectangle, including gesture preview.
 * Returns NOT_FOUND for a hidden, docked or natively floating panel.
 */
UmiStatus umi_gtk4_workspace_layout_host_canvas_geometry(
    const UmiGtk4WorkspaceLayoutHost *host,
    const char *window_id,
    UmiApplicationSuiteLayoutRect *out_rect);
/**
 * Render the supplied model on the GTK owning thread. Geometry-only changes
 * to existing canvas panels retain their native widgets and draft state;
 * structural, chrome or permission changes reconstruct the affected host.
 * When content retention is enabled, only the wrappers are reconstructed;
 * provider bodies stay alive until explicitly invalidated or host destruction.
 * Pending gestures/requests are cancelled before either update path. Rejected
 * model validation preserves the current rendered tree and pending request.
 */
UmiStatus umi_gtk4_workspace_layout_host_rebuild(
    UmiGtk4WorkspaceLayoutHost *host,
    const UmiUiWorkspaceLayout *layout);
/**
 * Provide the gtk4 workspace layout host widget operation used by this module and its
 * client applications.
 */
GtkWidget *umi_gtk4_workspace_layout_host_widget(
    UmiGtk4WorkspaceLayoutHost *host);
/**
 * Provide the gtk4 workspace layout host snapshot operation used by this module and its
 * client applications.
 */
UmiGtk4WorkspaceLayoutHostSnapshot umi_gtk4_workspace_layout_host_snapshot(
    const UmiGtk4WorkspaceLayoutHost *host);
/**
 * Return the number of portable layout-window records represented by the host.
 */
size_t umi_gtk4_workspace_layout_host_window_count(
    const UmiGtk4WorkspaceLayoutHost *host);
/**
 * Initialise gtk4 workspace layout placeholder from caller-provided values so later
 * operations receive a known state.
 */
GtkWidget *umi_gtk4_workspace_layout_placeholder_create(
    const char *title,
    const char *message);

#ifdef __cplusplus
}
#endif
#endif
