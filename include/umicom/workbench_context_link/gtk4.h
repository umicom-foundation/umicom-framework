/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/gtk4.h
 *
 * PURPOSE:
 *   Expose GTK4 widget factories for linked-workbench controls without moving semantic state into GTK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_GTK4_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_GTK4_H

#include <gtk/gtk.h>
#include "umicom/workbench_context_link/workbench_context_link.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the workbench context link gtk4 group picker new operation used by this module
 * and its client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_group_picker_new(
    const UmiWorkbenchContextLinkService *service);
/**
 * Provide the workbench context link gtk4 badge new operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_badge_new(
    const UmiWorkbenchContextLinkGroupProfile *profile);
/**
 * Provide the workbench context link gtk4 panel header new operation used by this module
 * and its client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_panel_header_new(
    const char *title,
    const UmiWorkbenchContextLinkGroupProfile *profile);
/**
 * Provide the workbench context link gtk4 inspector new operation used by this module and
 * its client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_inspector_new(
    const UmiWorkbenchContextLinkService *service,
    const char *group_id);
/**
 * Provide the workbench context link gtk4 history new operation used by this module and
 * its client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_history_new(
    const UmiWorkbenchContextLinkService *service);
/**
 * Provide the workbench context link gtk4 status new operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_status_new(
    const UmiWorkbenchContextLinkService *service);
/**
 * Provide the workbench context link gtk4 toolbar new operation used by this module and
 * its client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_toolbar_new(
    const UmiWorkbenchContextLinkService *service);
/**
 * Provide the workbench context link gtk4 panel host new operation used by this module and
 * its client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_panel_host_new(
    const char *panel_title,
    const UmiWorkbenchContextLinkGroupProfile *profile,
    GtkWidget *content);

#ifdef __cplusplus
}
#endif

#endif
