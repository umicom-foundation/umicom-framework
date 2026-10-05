/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/action_menu.h
 * PURPOSE: Group existing action buttons in a searchable native popover without copying their behavior.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_ACTION_MENU_H
#define UMICOM_UI_GTK4_ACTION_MENU_H
#include <gtk/gtk.h>
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Create a floating GtkMenuButton with a filter entry and scrollable actions.
 * Parent or ref-sink the result as with a normal GTK widget. Failure clears
 * out_menu. Use all operations on the GTK owner thread. The menu owns its
 * children; no application context, command registry or document is retained. */
    UmiStatus UmiGtk4ActionMenuCreate(const char *label, GtkWidget **out_menu);
    /* Append an unparented GtkButton, preserving its identity, sensitivity,
 * tooltip and existing callbacks. Normal GTK floating-reference rules apply.
 * A successful append parents the button and closes the popover on activation.
 * Label and optional keywords are copied for search. A menu accepts 256
 * actions; labels are at most 512 bytes and keywords at most 2048 bytes.
 * The host must not replace the service's popover or reparent its rows. */
    UmiStatus UmiGtk4ActionMenuAppend(GtkWidget *menu, GtkWidget *button, const char *keywords);
    /* Filter by a Unicode-normalized, case-insensitive substring of the copied
 * label and keywords. Empty text shows every action. Query is at most 1024
 * UTF-8 bytes and 256 characters. Filtering is coalesced on the GTK main loop;
 * VisibleCount describes the most recently displayed result, not queued work.
 * No action is automatically invoked when filtering or pressing Enter here. */
    UmiStatus UmiGtk4ActionMenuSetFilter(GtkWidget *menu, const char *query);
    size_t UmiGtk4ActionMenuVisibleCount(GtkWidget *menu);
    /* Borrow the filter entry while the menu remains alive. Hosts may use this
 * to focus it or add an automation identity; do not reparent the entry. */
    GtkWidget *UmiGtk4ActionMenuFilterEntry(GtkWidget *menu);
#ifdef __cplusplus
}
#endif
#endif
