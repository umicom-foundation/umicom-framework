/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_status_gtk4.c
 *
 * PURPOSE:
 *   Render save, revision, issue, collaborator and zoom evidence in the Layout
 *   Designer status area.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


GtkWidget *umi_workbench_designer_gtk4_build_status(
    UmiWorkbenchDesignerGtk4 *designer)
{
    GtkWidget *status = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    designer->status_label = gtk_label_new("Layout Designer ready");
    designer->zoom_label = gtk_label_new("100%");
    gtk_label_set_xalign(GTK_LABEL(designer->status_label), 0.0F);
    gtk_widget_set_hexpand(designer->status_label, TRUE);
    gtk_widget_add_css_class(status, "statusbar");
    gtk_box_append(GTK_BOX(status), designer->status_label);
    gtk_box_append(GTK_BOX(status), designer->zoom_label);
    return status;
}

void umi_workbench_designer_gtk4_refresh_status(
    UmiWorkbenchDesignerGtk4 *designer)
{
    char text[512];
    if (designer == NULL || designer->status_label == NULL) return;
    g_snprintf(
        text, sizeof(text),
        "%s · revision %" G_GUINT64_FORMAT " · %zu issues · %zu collaborators",
        umi_workbench_designer_save_state_text(
            designer->bundle.service_snapshot.save_state),
        (guint64)designer->bundle.service_snapshot.document_revision,
        designer->bundle.service_snapshot.issue_count,
        designer->bundle.service_snapshot.collaborator_count);
    gtk_label_set_text(GTK_LABEL(designer->status_label), text);
}
