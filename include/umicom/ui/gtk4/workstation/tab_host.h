/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/tab_host.h
 *
 * PURPOSE:
 *   Render semantic tab stacks as GTK4 notebook hosts with reusable document/tool-surface tab behaviour.
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

#include <gtk/gtk.h>
#include "umicom/ui/workstation/tab_stack.h"

/**
 * Initialise gtk4 ws tab host from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_tab_host_create(const UmiWsTabStack *stack);
/**
 * Add gtk4 ws tab host only after its inputs and available capacity have been checked.
 */
UmiStatus umi_gtk4_ws_tab_host_append(GtkWidget *host, const char *label, GtkWidget *child);

#endif
