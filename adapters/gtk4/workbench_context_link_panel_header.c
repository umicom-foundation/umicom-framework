/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_link_panel_header.c
 *
 * PURPOSE:
 *   Render the reusable GTK4 context-link panel header surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/gtk4.h"

GtkWidget *umi_workbench_context_link_gtk4_panel_header_new(
    const char *title,
    const UmiWorkbenchContextLinkGroupProfile *profile)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *label = gtk_label_new(title != NULL ? title : "Panel");
    GtkWidget *badge = umi_workbench_context_link_gtk4_badge_new(profile);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), badge);
    return box;
}
