/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * The host owns only composition and close approval. Framework owns all form,
 * persistence, browsing, history, recovery and conflict-review behavior. */
#include "schema.h"
#include "umicom/ui/gtk4/form_record_editor.h"
static gboolean close_requested(GtkWindow *window, gpointer data) {
  (void)window;
  return !UmiFormRecordEditorGtk4RequestClose(GTK_WIDGET(data));
}
static void activate(GtkApplication *application, gpointer data) {
  (void)data;
  GtkWindow *existing = gtk_application_get_active_window(application);
  if (existing != NULL) {
    gtk_window_present(existing);
    return;
  }
  GtkWidget *editor = NULL;
  UmiFormRecordEditorGtk4Options options = {.schema = &equipment_schema,
                                            .entity_prefix = "example.asset.",
                                            .source = "Equipment example"};
  UmiStatus status = UmiFormRecordEditorGtk4New(&options, &editor);
  if (status != UMI_STATUS_OK) {
    g_printerr("Could not create editor: %d\n", (int)status);
    g_object_set_data(G_OBJECT(application), "editor-failed", GINT_TO_POINTER(1));
    return;
  }
  GtkWidget *window = gtk_application_window_new(application);
  gtk_window_set_title(GTK_WINDOW(window), "Fictional equipment records");
  gtk_window_set_default_size(GTK_WINDOW(window), 900, 760);
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), editor);
  gtk_window_set_child(GTK_WINDOW(window), scroll);
  g_signal_connect_object(window, "close-request", G_CALLBACK(close_requested), editor, 0);
  gtk_window_present(GTK_WINDOW(window));
}
int main(int argc, char **argv) {
  GtkApplication *application =
      gtk_application_new("org.umicom.examples.form-records", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
  int result = g_application_run(G_APPLICATION(application), argc, argv);
  if (g_object_get_data(G_OBJECT(application), "editor-failed") != NULL)
    result = 1;
  g_object_unref(application);
  return result;
}
