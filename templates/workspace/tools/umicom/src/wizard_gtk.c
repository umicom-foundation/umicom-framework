/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/tools/umicom/src/wizard_gtk.c
 *
 * PURPOSE:
 *   Implement the wizard gtk behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "wizard.h"
#include "umicom/scaffold/scaffold.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

typedef struct WizardState {
    GtkWidget *window;
    GtkWidget *destination;
    GtkWidget *application_name;
    GtkWidget *console_check;
    GtkWidget *gtk_check;
    GtkWidget *web_check;
    GtkWidget *status;
    const char *template_root;
} WizardState;

/* Provide the show status operation used by this module and its client applications. */
static void show_status(WizardState *state, const char *message)
{
    gtk_label_set_text(GTK_LABEL(state->status), message);
}

/* Provide the create clicked operation used by this module and its client applications. */
static void create_clicked(GtkButton *button, gpointer user_data)
{
    WizardState *state = (WizardState *)user_data;
    const char *destination = gtk_editable_get_text(GTK_EDITABLE(state->destination));
    const char *name = gtk_editable_get_text(GTK_EDITABLE(state->application_name));
    UmiScaffoldRequest request;
    UmiScaffoldReport report;
    UmiStatus status;
    unsigned frontends = 0U;
    char message[1024];
    (void)button;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || destination[0] == '\0' || name == NULL || name[0] == '\0') {
        show_status(state, "Enter a destination folder and application name.");
        return;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (gtk_check_button_get_active(GTK_CHECK_BUTTON(state->console_check)))
        frontends |= UMI_SCAFFOLD_FRONTEND_CONSOLE;
    /* Apply this operation only while the related capability or state is available. */
    if (gtk_check_button_get_active(GTK_CHECK_BUTTON(state->gtk_check)))
        frontends |= UMI_SCAFFOLD_FRONTEND_GTK4;
    /* Apply this operation only while the related capability or state is available. */
    if (gtk_check_button_get_active(GTK_CHECK_BUTTON(state->web_check)))
        frontends |= UMI_SCAFFOLD_FRONTEND_WEB;
    /* Apply this branch only when its contract condition is satisfied. */
    if (frontends == 0U) {
        show_status(state, "Select at least one frontend.");
        return;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_scaffold_is_workspace(destination)) {
        status = umi_scaffold_copy_workspace_template(state->template_root, destination);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)snprintf(message, sizeof(message), "Workspace creation failed: %s",
                           umi_status_text(status));
            show_status(state, message);
            return;
        }
    }

    request.workspace_root = destination;
    request.application_name = name;
    request.frontends = frontends;
    status = umi_scaffold_create_application(&request, &report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)snprintf(message, sizeof(message), "Application creation failed: %s",
                       umi_status_text(status));
        show_status(state, message);
        return;
    }
    (void)snprintf(message, sizeof(message),
                   "Created %s with %zu files in %s",
                   report.application_slug, report.files_created,
                   report.application_directory);
    show_status(state, message);
}

/* Provide the labelled entry operation used by this module and its client applications. */
static GtkWidget *labelled_entry(GtkWidget *box, const char *label_text, const char *default_text)
{
    GtkWidget *label = gtk_label_new(label_text);
    GtkWidget *entry = gtk_entry_new();
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_editable_set_text(GTK_EDITABLE(entry), default_text);
    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), entry);
    return entry;
}

/* Provide the activate operation used by this module and its client applications. */
static void activate(GtkApplication *application, gpointer user_data)
{
    WizardState *state = (WizardState *)user_data;
    GtkWidget *box;
    GtkWidget *title;
    GtkWidget *frontends_label;
    GtkWidget *button;

    state->window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(state->window), "Umicom Framework Application Wizard");
    gtk_window_set_default_size(GTK_WINDOW(state->window), 720, 620);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 28);
    gtk_widget_set_margin_bottom(box, 28);
    gtk_widget_set_margin_start(box, 32);
    gtk_widget_set_margin_end(box, 32);

    title = gtk_label_new("Create an Umicom application");
    gtk_widget_add_css_class(title, "title-1");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_box_append(GTK_BOX(box), title);

    state->destination = labelled_entry(box, "Destination workspace folder", "C:/Dev/Umicom/UmicomStudioIDE");
    state->application_name = labelled_entry(box, "Application name", "Umicom Studio IDE");

    frontends_label = gtk_label_new("Frontends");
    gtk_label_set_xalign(GTK_LABEL(frontends_label), 0.0F);
    gtk_widget_add_css_class(frontends_label, "heading");
    gtk_box_append(GTK_BOX(box), frontends_label);

    state->console_check = gtk_check_button_new_with_label("Console");
    state->gtk_check = gtk_check_button_new_with_label("GTK4 desktop window");
    state->web_check = gtk_check_button_new_with_label("Web frontend and local C web server");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(state->console_check), TRUE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(state->gtk_check), TRUE);
    gtk_box_append(GTK_BOX(box), state->console_check);
    gtk_box_append(GTK_BOX(box), state->gtk_check);
    gtk_box_append(GTK_BOX(box), state->web_check);

    button = gtk_button_new_with_label("Create workspace and application");
    gtk_widget_add_css_class(button, "suggested-action");
    g_signal_connect(button, "clicked", G_CALLBACK(create_clicked), state);
    gtk_box_append(GTK_BOX(box), button);

    state->status = gtk_label_new("No files have been created yet.");
    gtk_label_set_wrap(GTK_LABEL(state->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(state->status), 0.0F);
    gtk_box_append(GTK_BOX(box), state->status);

    gtk_window_set_child(GTK_WINDOW(state->window), box);
    gtk_window_present(GTK_WINDOW(state->window));
}

/*
 * Perform wizard through the module contract so client applications do not duplicate its
 * policy.
 */
int umi_wizard_run(int argc, char **argv, const char *template_root)
{
    GtkApplication *application;
    WizardState state;
    int code;
    memset(&state, 0, sizeof(state));
    state.template_root = template_root;
    application = gtk_application_new("org.umicom.framework.wizard", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(application, "activate", G_CALLBACK(activate), &state);
    code = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return code;
}
