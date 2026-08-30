/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_review_gtk4.c
 *
 * PURPOSE:
 *   Render history, diagnostics and collaboration projections in a reusable GTK4
 *   review notebook.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


GtkWidget *umi_workbench_designer_gtk4_build_review(
    UmiWorkbenchDesignerGtk4 *designer)
{
    GtkWidget *notebook = gtk_notebook_new();
    designer->history_list = gtk_list_box_new();
    designer->diagnostics_list = gtk_list_box_new();
    designer->collaboration_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook),
        designer->history_list,
        gtk_label_new("History"));
    gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook),
        designer->diagnostics_list,
        gtk_label_new("Diagnostics"));
    gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook),
        designer->collaboration_box,
        gtk_label_new("Collaboration"));
    return notebook;
}

static void list_clear(GtkWidget *list)
{
    GtkWidget *child;
    if (list == NULL) return;
    child = gtk_widget_get_first_child(list);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(list), child);
        child = next;
    }
}

void umi_workbench_designer_gtk4_refresh_review(
    UmiWorkbenchDesignerGtk4 *designer)
{
    UmiWorkbenchDesignerSession *session;
    const UmiWorkbenchDesignerDiagnostics *diagnostics;
    const UmiWorkbenchDesignerCollaborationModel *collaboration;
    size_t index;
    if (designer == NULL) return;
    list_clear(designer->diagnostics_list);
    list_clear(designer->history_list);
    umi_workbench_designer_gtk4_clear_box(designer->collaboration_box);
    session = umi_workbench_designer_service_active(
        designer->config.controller->service);
    if (session == NULL) return;
    diagnostics = umi_workbench_designer_session_diagnostics(session);
    if (diagnostics != NULL) {
        for (index = 0U; index < diagnostics->count; ++index) {
            const UmiWorkbenchDesignerIssue *issue = &diagnostics->issues[index];
            GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
            GtkWidget *message = gtk_label_new(issue->message);
            GtkWidget *remediation = gtk_label_new(issue->remediation);
            gtk_label_set_wrap(GTK_LABEL(message), TRUE);
            gtk_label_set_wrap(GTK_LABEL(remediation), TRUE);
            gtk_label_set_xalign(GTK_LABEL(message), 0.0F);
            gtk_label_set_xalign(GTK_LABEL(remediation), 0.0F);
            gtk_widget_add_css_class(remediation, "dim-label");
            gtk_box_append(GTK_BOX(row), message);
            gtk_box_append(GTK_BOX(row), remediation);
            gtk_list_box_append(GTK_LIST_BOX(designer->diagnostics_list), row);
        }
    }
    collaboration = umi_workbench_designer_session_collaboration(session);
    if (collaboration != NULL) {
        for (index = 0U; index < collaboration->count; ++index) {
            const UmiWorkbenchDesignerCollaborator *person =
                &collaboration->collaborators[index];
            GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
            GtkWidget *name = gtk_label_new(person->display_name);
            GtkWidget *state = gtk_label_new(
                person->state == UMI_WORKBENCH_DESIGNER_COLLABORATOR_EDITING
                    ? "Editing" : "Viewing");
            gtk_label_set_xalign(GTK_LABEL(name), 0.0F);
            gtk_widget_set_hexpand(name, TRUE);
            gtk_box_append(GTK_BOX(row), name);
            gtk_box_append(GTK_BOX(row), state);
            gtk_box_append(GTK_BOX(designer->collaboration_box), row);
        }
    }
}
