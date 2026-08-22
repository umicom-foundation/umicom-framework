/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_link_badge.c
 *
 * PURPOSE:
 *   Render the reusable GTK4 context-link badge surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/gtk4.h"

GtkWidget *umi_workbench_context_link_gtk4_badge_new(
    const UmiWorkbenchContextLinkGroupProfile *profile)
{
    GtkWidget *label;
    const char *text = "Unlinked";
    if (profile != NULL) {
        text = profile->title[0] != '\0' ? profile->title : profile->group_id;
    }
    label = gtk_label_new(text);
    gtk_widget_add_css_class(label, "umicom-context-link-badge");
    if (profile != NULL) {
        gtk_widget_set_tooltip_text(label, umi_context_colour_text(profile->colour));
    }
    return label;
}
