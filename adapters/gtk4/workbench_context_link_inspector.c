/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_link_inspector.c
 *
 * PURPOSE:
 *   Render the reusable GTK4 context-link inspector surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/gtk4.h"

GtkWidget *umi_workbench_context_link_gtk4_inspector_new(
    const UmiWorkbenchContextLinkService *service,
    const char *group_id)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    const UmiWorkbenchContextLinkActiveSlot *slot = NULL;
    GtkWidget *title = gtk_label_new("Context Inspector");
    gtk_widget_add_css_class(title, "heading");
    gtk_box_append(GTK_BOX(box), title);
    if (service != NULL && group_id != NULL) {
        slot = umi_workbench_context_link_service_current(service, group_id);
    }
    if (slot == NULL) {
        gtk_box_append(GTK_BOX(box), gtk_label_new("No active context"));
    } else {
        gtk_box_append(GTK_BOX(box), gtk_label_new(umi_context_kind_text(slot->payload.kind)));
        gtk_box_append(GTK_BOX(box), gtk_label_new(slot->payload.identity.context_id));
        gtk_box_append(GTK_BOX(box), gtk_label_new(slot->payload.identity.source_application_id));
    }
    return box;
}
