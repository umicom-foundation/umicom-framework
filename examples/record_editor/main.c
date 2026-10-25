/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/record_editor/main.c
 *
 * PURPOSE:
 *   Installed-SDK native host. Only product identity and window composition live
 *   here; Framework owns draft editing, revision checks and storage workers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Installed-SDK native host. Only product identity and window composition live
 * here; Framework owns draft editing, revision checks and storage workers. */
#include "umicom/ui/gtk4/record_editor.h"
static gboolean close_requested(GtkWindow *window, gpointer data)
{
    (void)window;
    return !UmiRecordEditorGtk4RequestClose(GTK_WIDGET(data));
}
static void release_editor(gpointer data, GClosure *closure)
{
    (void)closure;
    g_object_unref(data);
}
static void activate(GtkApplication *application, gpointer data)
{
    (void)data;
    GtkWindow *existing = gtk_application_get_active_window(application);
    if (existing != NULL) { gtk_window_present(existing); return; }
    const UmiRecordEditorGtk4Options options = {
        .title = "Local record notes",
        .description = "Practice saving and reopening fictional notes. Choose an explicit local database; no network connection is used.",
        .entity_prefix = "example.review.", .source = "Umicom SDK example"};
    GtkWidget *editor = NULL;
    UmiStatus status = UmiRecordEditorGtk4New(&options, &editor);
    GtkWidget *window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(window), "Umicom record editor example");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 850);
    if (status != UMI_STATUS_OK) editor = gtk_label_new("Record editor unavailable.");
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), editor);
    gtk_window_set_child(GTK_WINDOW(window), scroll);
    if (status == UMI_STATUS_OK) g_signal_connect_data(window, "close-request",
        G_CALLBACK(close_requested), g_object_ref(editor), release_editor, 0);
    gtk_window_present(GTK_WINDOW(window));
}
int main(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new("org.umicom.recordeditor.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
    int result = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return result;
}
