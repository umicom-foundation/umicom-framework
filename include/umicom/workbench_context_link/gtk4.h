/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/gtk4.h
 *
 * PURPOSE:
 *   Expose GTK4 widget factories for linked-workbench controls without moving semantic state into GTK.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_GTK4_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_GTK4_H

#include <gtk/gtk.h>
#include "umicom/workbench_context_link/workbench_context_link.h"

#ifdef __cplusplus
extern "C" {
#endif

GtkWidget *umi_workbench_context_link_gtk4_group_picker_new(
    const UmiWorkbenchContextLinkService *service);
GtkWidget *umi_workbench_context_link_gtk4_badge_new(
    const UmiWorkbenchContextLinkGroupProfile *profile);
GtkWidget *umi_workbench_context_link_gtk4_panel_header_new(
    const char *title,
    const UmiWorkbenchContextLinkGroupProfile *profile);
GtkWidget *umi_workbench_context_link_gtk4_inspector_new(
    const UmiWorkbenchContextLinkService *service,
    const char *group_id);
GtkWidget *umi_workbench_context_link_gtk4_history_new(
    const UmiWorkbenchContextLinkService *service);
GtkWidget *umi_workbench_context_link_gtk4_status_new(
    const UmiWorkbenchContextLinkService *service);
GtkWidget *umi_workbench_context_link_gtk4_toolbar_new(
    const UmiWorkbenchContextLinkService *service);
GtkWidget *umi_workbench_context_link_gtk4_panel_host_new(
    const char *panel_title,
    const UmiWorkbenchContextLinkGroupProfile *profile,
    GtkWidget *content);

#ifdef __cplusplus
}
#endif

#endif
