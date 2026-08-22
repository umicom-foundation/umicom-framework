/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_link_toolbar.c
 *
 * PURPOSE:
 *   Render the reusable GTK4 context-link toolbar surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/gtk4.h"

GtkWidget *umi_workbench_context_link_gtk4_toolbar_new(
    const UmiWorkbenchContextLinkService *service)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *back = gtk_button_new_with_label("Back");
    GtkWidget *forward = gtk_button_new_with_label("Forward");
    GtkWidget *pin = gtk_button_new_with_label("Pin");
    GtkWidget *groups = umi_workbench_context_link_gtk4_group_picker_new(service);
    gtk_box_append(GTK_BOX(box), back);
    gtk_box_append(GTK_BOX(box), forward);
    gtk_box_append(GTK_BOX(box), pin);
    gtk_box_append(GTK_BOX(box), groups);
    return box;
}
