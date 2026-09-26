/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/education_workspace_application_gtk4.c
 * PURPOSE: Compose a learning surface without replacing the existing host.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "education_workspace_private.h"
#include "umicom/application/suite_layout/gtk4_product_content_extension.h"

typedef struct EducationHost {
    UmiEducationGtkPanel *panel;
    GtkWidget *root;
    GtkWidget *original;
    GtkStack *stack;
    GtkScrolledWindow *scroll;
} EducationHost;

static UmiStatus Build(GtkWindow *window,
    UmiApplicationProductGtk4Workstation *workstation,
    GtkWidget *original, void *context, GtkWidget **outContent)
{
    EducationHost *host = context;
    (void)workstation;
    if (host == NULL || original == NULL || outContent == NULL || host->root != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outContent = NULL;
    host->panel = UmiEducationGtkCreate();
    if (host->panel == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    host->root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));
    host->stack = GTK_STACK(gtk_stack_new());
    GtkWidget *switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), host->stack);
    host->scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
    gtk_scrolled_window_set_policy(host->scroll, GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(host->scroll, UmiEducationGtkWidget(host->panel));
    gtk_stack_add_titled(host->stack, GTK_WIDGET(host->scroll), "learning", "Learning practicum");
    gtk_stack_add_titled(host->stack, original, "existing", "Existing layouts");
    host->original = original;
    gtk_widget_set_vexpand(GTK_WIDGET(host->stack), TRUE);
    gtk_box_append(GTK_BOX(host->root), switcher);
    gtk_box_append(GTK_BOX(host->root), GTK_WIDGET(host->stack));
    UmiEducationGtkAttachCloseGuard(host->panel, window);
    *outContent = host->root;
    return UMI_STATUS_OK;
}

/* Retain the actual scrolled window, not the panel's immediate parent: GTK
 * inserts a viewport around non-scrollable children. Clear the scroll child
 * before releasing the panel, then detach the borrowed original workstation. */
static void Release(void *context)
{
    EducationHost *host = context;
    if (host == NULL) return;
    if (host->scroll != NULL) gtk_scrolled_window_set_child(host->scroll, NULL);
    if (host->original != NULL &&
        gtk_widget_get_parent(host->original) == GTK_WIDGET(host->stack)) {
        gtk_stack_remove(host->stack, host->original);
    }
    UmiEducationGtkDestroy(host->panel);
    if (host->root != NULL) g_object_unref(host->root);
    *host = (EducationHost){0};
}

int UmiEducationGtkRun(const char *id, const char *title, int argc, char **argv)
{
    EducationHost host = {0};
    UmiApplicationProductGtk4WorkstationConfig config =
        umi_application_product_gtk4_workstation_config_default(id, title, NULL, NULL);
    UmiApplicationProductGtk4ContentExtension extension = {Build, Release, &host};
    config.mode_badge = "Self-study practicum";
    int result = umi_application_product_gtk4_run_with_content(&config, &extension, argc, argv);
    Release(&host);
    return result;
}

typedef struct EducationDialog {
    UmiEducationGtkPanel *panel;
    GtkScrolledWindow *scroll;
} EducationDialog;

static void DisposeDialog(gpointer context)
{
    EducationDialog *dialog = context;
    /* Own the scroll reference until this cleanup: it may already have been
     * detached from its window during native destruction. */
    gtk_scrolled_window_set_child(dialog->scroll, NULL);
    UmiEducationGtkDestroy(dialog->panel);
    g_object_unref(dialog->scroll);
    g_free(dialog);
}

void UmiEducationGtkOpen(GtkWindow *parent)
{
    UmiEducationGtkPanel *panel = UmiEducationGtkCreate();
    if (panel == NULL) return;
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Umicom Education — Learning practicum");
    gtk_window_set_default_size(GTK_WINDOW(window), 880, 720);
    if (parent != NULL) {
        gtk_window_set_transient_for(GTK_WINDOW(window), parent);
        /* Do not force-destroy a learning window when the Studio parent closes:
         * the learner must still be able to save or discard a draft note. */
        GtkApplication *application = gtk_window_get_application(parent);
        if (application != NULL) gtk_window_set_application(GTK_WINDOW(window), application);
    }
    EducationDialog *dialog = g_new0(EducationDialog, 1U);
    dialog->panel = panel;
    dialog->scroll = g_object_ref_sink(GTK_SCROLLED_WINDOW(gtk_scrolled_window_new()));
    gtk_scrolled_window_set_policy(dialog->scroll, GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(dialog->scroll, UmiEducationGtkWidget(panel));
    gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(dialog->scroll));
    UmiEducationGtkAttachCloseGuard(panel, GTK_WINDOW(window));
    g_object_set_data_full(G_OBJECT(window), "umicom-education-dialog", dialog, DisposeDialog);
    gtk_window_present(GTK_WINDOW(window));
}

static void Launch(GtkButton *button, gpointer context)
{
    (void)context;
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    UmiEducationGtkOpen(GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL);
}

GtkWidget *UmiEducationGtkLauncher(void)
{
    GtkWidget *button = gtk_button_new_with_label("Open learning practicum and saved progress");
    g_object_set_data_full(G_OBJECT(button), "umicom-automation-id", g_strdup("education.launcher"), g_free);
    g_signal_connect(button, "clicked", G_CALLBACK(Launch), NULL);
    return button;
}
