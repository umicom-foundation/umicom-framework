/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/command_bar_gtk4.c
 *
 * PURPOSE:
 *   Render the unified command/search centre with visible scope affordances for commands, panels, settings, symbols and AI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/command_bar.h"

GtkWidget *umi_gtk4_ws_command_bar_create(const char *placeholder) {
    GtkWidget *entry = gtk_search_entry_new();
    gtk_widget_add_css_class(entry, "umicom-command-bar");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder != NULL ? placeholder : "Search commands, panels, layouts, settings, symbols and AI…");
    gtk_widget_set_hexpand(entry, TRUE);
    return entry;
}

UmiStatus umi_gtk4_ws_command_bar_query(GtkWidget *entry, UmiWsCommandBarQuery *out_query) {
    const char *text;
    if (entry == NULL || !GTK_IS_EDITABLE(entry) || out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    text = gtk_editable_get_text(GTK_EDITABLE(entry));
    return umi_ws_command_bar_parse(text, out_query);
}
