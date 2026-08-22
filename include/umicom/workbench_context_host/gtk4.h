/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/gtk4.h
 *
 * PURPOSE:
 *   Expose reusable GTK4 controls that render the toolkit-neutral Workbench Context Host.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_GTK4_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_GTK4_H
#include <gtk/gtk.h>
#include "umicom/workbench_context_host/workbench_context_host.h"
#ifdef __cplusplus
extern "C" {
#endif
GtkWidget *umi_workbench_context_host_gtk4_strip_new(UmiWorkbenchContextHost *host);
UmiStatus umi_workbench_context_host_gtk4_strip_refresh(
    GtkWidget *strip,UmiWorkbenchContextHost *host);
GtkWidget *umi_workbench_context_host_gtk4_group_picker_new(UmiWorkbenchContextHost *host);
UmiStatus umi_workbench_context_host_gtk4_group_picker_refresh(
    GtkWidget *picker,UmiWorkbenchContextHost *host);
GtkWidget *umi_workbench_context_host_gtk4_status_new(UmiWorkbenchContextHost *host);
UmiStatus umi_workbench_context_host_gtk4_status_refresh(
    GtkWidget *status,UmiWorkbenchContextHost *host);
GtkWidget *umi_workbench_context_host_gtk4_inspector_new(
    UmiWorkbenchContextHost *host,const char *group_id);
UmiStatus umi_workbench_context_host_gtk4_inspector_refresh(
    GtkWidget *inspector,UmiWorkbenchContextHost *host,const char *group_id);
GtkWidget *umi_workbench_context_host_gtk4_endpoint_list_new(UmiWorkbenchContextHost *host);
GtkWidget *umi_workbench_context_host_gtk4_history_new(
    UmiWorkbenchContextHost *host,const char *group_id);
GtkWidget *umi_workbench_context_host_gtk4_inbox_new(UmiWorkbenchContextHost *host);
GtkWidget *umi_workbench_context_host_gtk4_panel_badge_new(
    UmiWorkbenchContextHost *host,const char *endpoint_id);
UmiStatus umi_workbench_context_host_gtk4_panel_badge_refresh(
    GtkWidget *badge,UmiWorkbenchContextHost *host,const char *endpoint_id);
#ifdef __cplusplus
}
#endif
#endif
