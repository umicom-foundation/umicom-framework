/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_strip_gtk4.c
 *
 * PURPOSE:
 *   Render the saved-layout/workspace strip used for rapid perspective/layout switching across workstation applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/workspace_strip.h"

/*
 * Initialise gtk4 ws workspace strip from caller-provided values so later operations
 * receive a known state.
 */
GtkWidget *umi_gtk4_ws_workspace_strip_create(const UmiWsWorkspaceStrip *strip) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    size_t i;
    gtk_widget_add_css_class(box, "umicom-workspace-strip");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strip == NULL) return box;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < strip->count; ++i) {
        GtkWidget *button = gtk_toggle_button_new_with_label(strip->entries[i].label);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), i == strip->active_index);
        gtk_widget_set_tooltip_text(button, strip->entries[i].layout_id);
        gtk_widget_add_css_class(button, "umicom-workspace-tab");
        gtk_box_append(GTK_BOX(box), button);
    }
    return box;
}
