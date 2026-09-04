/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/workspace_layout_host.h
 *
 * PURPOSE:
 *   Render canonical UmiUiWorkspaceLayout geometry through the existing GTK4
 *   workstation tab, panel, split and docking primitives.
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
 * Release or reset state held by gtk4 workspace layout host so the same storage can be
 * reused safely.
 */
void umi_gtk4_workspace_layout_host_destroy(
    UmiGtk4WorkspaceLayoutHost *host);
/**
 * Set the optional native parent used by subsequently created floating panel windows.
 */
void umi_gtk4_workspace_layout_host_set_transient_parent(
    UmiGtk4WorkspaceLayoutHost *host,
    GtkWindow *parent);
/**
 * Provide the gtk4 workspace layout host rebuild operation used by this module and its
 * client applications.
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
