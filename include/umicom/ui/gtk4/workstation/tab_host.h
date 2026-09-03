/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/tab_host.h
 *
 * PURPOSE:
 *   Present reusable workstation tab groups with readable labels, optional
 *   close actions and source-compatible simple append behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_TAB_HOST_H
#define UMICOM_UI_GTK4_WORKSTATION_TAB_HOST_H

#include <stdbool.h>

#include <gtk/gtk.h>
#include "umicom/ui/workstation/tab_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Called when a managed tab asks its layout owner to close the tab. */
typedef void (*UmiGtk4WorkstationTabCloseHandler)(
    const char *tab_id,
    void *user_data);

/** Create a reusable scrollable workstation tab host. */
GtkWidget *umi_gtk4_ws_tab_host_create(
    const UmiWsTabStack *stack);

/**
 * Append one simple non-closable tab.
 *
 * This original API is retained for source compatibility. New layout hosts
 * should use `append_managed` when they own a stable tab identifier.
 */
UmiStatus umi_gtk4_ws_tab_host_append(
    GtkWidget *notebook,
    const char *title,
    GtkWidget *child);

/**
 * Append one managed tab with an optional close action.
 *
 * The host copies `tab_id` into button-owned state. The child becomes owned by
 * the notebook after a successful append.
 */
UmiStatus umi_gtk4_ws_tab_host_append_managed(
    GtkWidget *notebook,
    const char *tab_id,
    const char *title,
    GtkWidget *child,
    bool closable,
    UmiGtk4WorkstationTabCloseHandler close_handler,
    void *user_data);

#ifdef __cplusplus
}
#endif

#endif
