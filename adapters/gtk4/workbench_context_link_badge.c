/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_link_badge.c
 *
 * PURPOSE:
 *   Render the reusable GTK4 context-link badge surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/gtk4.h"

/*
 * Provide the workbench context link gtk4 badge new operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_badge_new(
    const UmiWorkbenchContextLinkGroupProfile *profile)
{
    GtkWidget *label;
    const char *text = "Unlinked";
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile != NULL) {
        text = profile->title[0] != '\0' ? profile->title : profile->group_id;
    }
    label = gtk_label_new(text);
    gtk_widget_add_css_class(label, "umicom-context-link-badge");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile != NULL) {
        gtk_widget_set_tooltip_text(label, umi_context_colour_text(profile->colour));
    }
    return label;
}
