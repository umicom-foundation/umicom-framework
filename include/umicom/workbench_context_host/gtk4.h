/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/gtk4.h
 *
 * PURPOSE:
 *   Expose reusable GTK4 controls that render the toolkit-neutral Workbench Context Host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_GTK4_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_GTK4_H
#include <gtk/gtk.h>
#include "umicom/workbench_context_host/workbench_context_host.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the workbench context host gtk4 strip new operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_context_host_gtk4_strip_new(UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host gtk4 strip refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_gtk4_strip_refresh(
    GtkWidget *strip,UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host gtk4 group picker new operation used by this module
 * and its client applications.
 */
GtkWidget *umi_workbench_context_host_gtk4_group_picker_new(UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host gtk4 group picker refresh operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_gtk4_group_picker_refresh(
    GtkWidget *picker,UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host gtk4 status new operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_context_host_gtk4_status_new(UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host gtk4 status refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_gtk4_status_refresh(
    GtkWidget *status,UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host gtk4 inspector new operation used by this module and
 * its client applications.
 */
GtkWidget *umi_workbench_context_host_gtk4_inspector_new(
    UmiWorkbenchContextHost *host,const char *group_id);
/**
 * Provide the workbench context host gtk4 inspector refresh operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_gtk4_inspector_refresh(
    GtkWidget *inspector,UmiWorkbenchContextHost *host,const char *group_id);
/**
 * Provide the workbench context host gtk4 endpoint list new operation used by this module
 * and its client applications.
 */
GtkWidget *umi_workbench_context_host_gtk4_endpoint_list_new(UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host gtk4 history new operation used by this module and
 * its client applications.
 */
GtkWidget *umi_workbench_context_host_gtk4_history_new(
    UmiWorkbenchContextHost *host,const char *group_id);
/**
 * Provide the workbench context host gtk4 inbox new operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_context_host_gtk4_inbox_new(UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host gtk4 panel badge new operation used by this module
 * and its client applications.
 */
GtkWidget *umi_workbench_context_host_gtk4_panel_badge_new(
    UmiWorkbenchContextHost *host,const char *endpoint_id);
/**
 * Provide the workbench context host gtk4 panel badge refresh operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_gtk4_panel_badge_refresh(
    GtkWidget *badge,UmiWorkbenchContextHost *host,const char *endpoint_id);
#ifdef __cplusplus
}
#endif
#endif
