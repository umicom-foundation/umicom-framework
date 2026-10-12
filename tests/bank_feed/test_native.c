/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_native.c
 * PURPOSE: Exercise bank import publication and retained-window behavior without network access.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/bank_feed.h"
#include <glib/gstdio.h>
static GtkWidget *Find(GtkWindow *window, const char *name) {
  return umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
}
/* Inspect GTK's public window list, not object finalisation: tests retain
 * windows deliberately to catch controls that outlive their native parent. */
static bool Registered(GtkWindow *window) {
  GListModel *windows = gtk_window_get_toplevels();
  for (guint i = 0; i < g_list_model_get_n_items(windows); ++i) {
    gpointer item = g_list_model_get_item(windows, i);
    bool found = item == window;
    g_object_unref(item);
    if (found)
      return true;
  }
  return false;
}
static char *ViewText(GtkWidget *view) {
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(buffer, &start, &end);
  return gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
}
static bool Wait(GtkWindow *window) {
  gint64 end = g_get_monotonic_time() + 5000000;
  while (UmiBankFeedGtk4Busy(window) && g_get_monotonic_time() < end) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000U);
  }
  return !UmiBankFeedGtk4Busy(window);
}
typedef struct Reentry {
  GtkWindow *window;
  GtkWidget *clear;
  bool close;
  unsigned calls;
} Reentry;
static void Changed(GObject *object, GParamSpec *property, gpointer data) {
  (void)object;
  (void)property;
  Reentry *probe = data;
  if (probe->calls++)
    return;
  if (probe->close)
    gtk_window_destroy(probe->window);
  else
    g_signal_emit_by_name(probe->clear, "clicked");
}
/* Path and window changes during preparation must prevent worker dispatch,
 * even though no asynchronous result exists yet to invalidate. */
static void Prepared(GObject *object, GParamSpec *property, gpointer data) {
  (void)property;
  Reentry *probe = data;
  if (strstr(gtk_label_get_text(GTK_LABEL(object)), "Reading selected file") == NULL ||
      probe->calls++)
    return;
  if (probe->close)
    gtk_window_destroy(probe->window);
  else
    gtk_editable_set_text(GTK_EDITABLE(Find(probe->window, "bank-feed-path")),
                          "changed-before-dispatch.json");
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  if (!gtk_init_check())
    return 77;
  const char *mode = argv[1];
  GtkWindow *window = UmiBankFeedGtk4Create(NULL);
  CHECK(window);
  g_object_ref(window);
  GtkWidget *path = Find(window, "bank-feed-path"), *load = Find(window, "bank-feed-load"),
            *clear = Find(window, "bank-feed-clear"), *view = Find(window, "bank-feed-output"),
            *groups = Find(window, "bank-feed-groups"), *save = Find(window, "bank-feed-export"),
            *note = Find(window, "bank-feed-note");
  CHECK(path && load && clear && view && groups && save && note);
  g_object_ref(load);
  g_object_ref(clear);
  g_object_ref(view);
  g_object_ref(groups);
  g_object_ref(save);
  g_object_ref(note);
  if (!strcmp(mode, "fields")) {
    CHECK(!gtk_widget_get_sensitive(save) && !UmiBankFeedGtk4Busy(window));
  } else if (!strcmp(mode, "retained")) {
    gtk_window_destroy(window);
    g_signal_emit_by_name(load, "clicked");
    g_signal_emit_by_name(clear, "clicked");
    CHECK(!UmiBankFeedGtk4Busy(window));
    char *text = ViewText(view);
    CHECK(text[0] == '\0');
    g_free(text);
  } else if (!strcmp(mode, "parent")) {
    GtkWindow *child = UmiBankFeedGtk4Create(window);
    CHECK(child);
    g_object_ref(child);
    gtk_window_destroy(window);
    CHECK(UmiBankFeedGtk4Create(window) == NULL);
    CHECK(!Registered(child));
    g_object_unref(child);
  } else {
    GError *error = NULL;
    gchar *directory = g_dir_make_tmp("umicom-bank-feed-XXXXXX", &error);
    CHECK(directory && error == NULL);
    gchar *filename = g_build_filename(directory, "transactions.json", NULL);
    char *document = BankFixture(BANK_TEST_ROW);
    CHECK(document);
    CHECK(g_file_set_contents(filename, !strcmp(mode, "invalid") ? "{" : document, -1, &error));
    free(document);
    gtk_editable_set_text(GTK_EDITABLE(path), filename);
    bool preparation = !strcmp(mode, "prepare-close") || !strcmp(mode, "prepare-path");
    Reentry probe = {window, clear, !strcmp(mode, "close-notify") || !strcmp(mode, "prepare-close"),
                     0U};
    bool reentrant = !strcmp(mode, "clear-notify") || !strcmp(mode, "close-notify");
    gulong signal = 0U;
    if (preparation)
      signal = g_signal_connect(note, "notify::label", G_CALLBACK(Prepared), &probe);
    g_signal_emit_by_name(load, "clicked");
    CHECK(preparation ? !UmiBankFeedGtk4Busy(window) : UmiBankFeedGtk4Busy(window));
    if (!strcmp(mode, "path-change"))
      gtk_editable_set_text(GTK_EDITABLE(path), "changed.json");
    else if (!strcmp(mode, "close-loading"))
      gtk_window_destroy(window);
    else if (reentrant)
      signal = g_signal_connect(groups, "notify::selected", G_CALLBACK(Changed), &probe);
    CHECK(Wait(window));
    char *text = ViewText(view);
    if (!strcmp(mode, "invalid")) {
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(note)), "Import failed"));
      CHECK(!gtk_widget_get_sensitive(save));
    } else if (!strcmp(mode, "path-change") || !strcmp(mode, "close-loading") || reentrant ||
               preparation) {
      CHECK(!strstr(text, "Example credit"));
      if (reentrant || preparation)
        CHECK(probe.calls > 0U);
    } else {
      CHECK(strstr(text, "Booked credits: 12.50") && strstr(text, "Example credit"));
      CHECK(gtk_widget_get_sensitive(save));
      if (!strcmp(mode, "clear")) {
        g_signal_emit_by_name(clear, "clicked");
        g_free(text);
        text = ViewText(view);
        CHECK(text[0] == '\0' && !gtk_widget_get_sensitive(save));
      } else
        CHECK(!strcmp(mode, "import"));
    }
    if (signal)
      g_signal_handler_disconnect(preparation ? note : groups, signal);
    g_free(text);
    CHECK(g_remove(filename) == 0 && g_rmdir(directory) == 0);
    g_free(filename);
    g_free(directory);
    g_clear_error(&error);
  }
  gtk_window_destroy(window);
  g_object_unref(window);
  g_object_unref(load);
  g_object_unref(clear);
  g_object_unref(view);
  g_object_unref(groups);
  g_object_unref(save);
  g_object_unref(note);
  return 0;
}
