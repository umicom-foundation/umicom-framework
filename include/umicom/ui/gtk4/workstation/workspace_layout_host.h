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

#include <gtk/gtk.h>

#include "umicom/application/suite_layout/render_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef GtkWidget *(*UmiGtk4WorkspaceLayoutPanelFactory)(
    const UmiUiWorkspaceWindow *window,
    void *user_data);

typedef struct UmiGtk4WorkspaceLayoutHostSnapshot {
    char layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    size_t stack_count;
    size_t panel_count;
    size_t placeholder_count;
    size_t floating_count;
    uint64_t revision;
} UmiGtk4WorkspaceLayoutHostSnapshot;

typedef struct UmiGtk4WorkspaceLayoutHost UmiGtk4WorkspaceLayoutHost;

UmiStatus umi_gtk4_workspace_layout_host_create(
    const UmiUiWorkspaceLayout *layout,
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory,
    void *user_data,
    UmiGtk4WorkspaceLayoutHost **out_host);
void umi_gtk4_workspace_layout_host_destroy(
    UmiGtk4WorkspaceLayoutHost *host);
UmiStatus umi_gtk4_workspace_layout_host_rebuild(
    UmiGtk4WorkspaceLayoutHost *host,
    const UmiUiWorkspaceLayout *layout);
GtkWidget *umi_gtk4_workspace_layout_host_widget(
    UmiGtk4WorkspaceLayoutHost *host);
UmiGtk4WorkspaceLayoutHostSnapshot umi_gtk4_workspace_layout_host_snapshot(
    const UmiGtk4WorkspaceLayoutHost *host);
GtkWidget *umi_gtk4_workspace_layout_placeholder_create(
    const char *title,
    const char *message);

#ifdef __cplusplus
}
#endif
#endif
