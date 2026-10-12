/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_statement_native.c
 * PURPOSE: Check snapshot ownership and export invalidation through native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/bank_statement.h"
static GtkWidget *Find(GtkWindow *window, const char *name) {
  return umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
}
typedef struct StatementProbe {
  GtkWindow *window;
  GtkWidget *closing;
  bool close;
  unsigned calls;
} StatementProbe;
/* Mutate inputs at publication time, when stale result bugs are easiest to miss.
 * The first matching notification acts once; later invalidation must finish. */
static void Reenter(GObject *object, GParamSpec *spec, gpointer data) {
  (void)spec;
  StatementProbe *probe = data;
  if (GTK_IS_LABEL(object) &&
      strstr(gtk_label_get_text(GTK_LABEL(object)), "Arithmetic matches") == NULL)
    return;
  if (probe->calls++)
    return;
  if (probe->close)
    gtk_window_destroy(probe->window);
  else
    gtk_editable_set_text(GTK_EDITABLE(probe->closing), "999");
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  if (!gtk_init_check())
    return 77;
  char *document = BankFixture(BANK_TEST_ROW);
  CHECK(document);
  UmiBankFeed *feed = NULL;
  CHECK(UmiBankFeedCreate(document, strlen(document), &feed) == UMI_STATUS_OK);
  free(document);
  GtkWindow *parent = GTK_WINDOW(gtk_window_new());
  g_object_ref(parent);
  GtkWindow *window = UmiBankStatementGtk4Create(feed, "practice-account", "GBP", parent);
  CHECK(window);
  g_object_ref(window);
  CHECK(UmiBankStatementGtk4Create(feed, "missing", "GBP", parent) == NULL);
  UmiBankFeedDestroy(feed); /* The window must now use its own snapshot. */
  GtkWidget *first = Find(window, "statement-first"), *last = Find(window, "statement-last"),
            *opening = Find(window, "statement-opening"),
            *closing = Find(window, "statement-closing"),
            *compare = Find(window, "statement-compare"),
            *result = Find(window, "statement-result"), *save = Find(window, "statement-export");
  CHECK(first && last && opening && closing && compare && result && save);
  g_object_ref(compare);
  g_object_ref(result);
  g_object_ref(save);
  CHECK(!gtk_widget_get_sensitive(save));
  gtk_editable_set_text(GTK_EDITABLE(first), "2026-10-12");
  gtk_editable_set_text(GTK_EDITABLE(last), "2026-10-12");
  gtk_editable_set_text(GTK_EDITABLE(opening), "100.00");
  gtk_editable_set_text(GTK_EDITABLE(closing), "112.50");
  const char *mode = argv[1];
  bool reentrant = !strcmp(mode, "edit-notify") || !strcmp(mode, "close-notify");
  StatementProbe probe = {window, closing, !strcmp(mode, "close-notify"), 0U};
  gulong signal = 0U;
  if (reentrant)
    signal = g_signal_connect(result, "notify::label", G_CALLBACK(Reenter), &probe);
  if (!strcmp(mode, "retained") || !strcmp(mode, "parent")) {
    if (!strcmp(mode, "parent"))
      gtk_window_destroy(parent);
    else
      gtk_window_destroy(window);
    g_signal_emit_by_name(compare, "clicked");
    CHECK(!gtk_widget_get_sensitive(save));
  } else {
    if (!strcmp(mode, "invalid"))
      gtk_editable_set_text(GTK_EDITABLE(first), "2026-02-30");
    g_signal_emit_by_name(compare, "clicked");
    if (reentrant) {
      CHECK(probe.calls > 0U);
      if (!probe.close) {
        CHECK(!gtk_widget_get_sensitive(save));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(result)), "Enter dates"));
      }
    } else if (!strcmp(mode, "invalid")) {
      CHECK(!gtk_widget_get_sensitive(save));
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(result)), "Comparison unavailable"));
    } else {
      CHECK(gtk_widget_get_sensitive(save));
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(result)), "Arithmetic matches"));
      if (!strcmp(mode, "edit")) {
        gtk_editable_set_text(GTK_EDITABLE(closing), "112.51");
        CHECK(!gtk_widget_get_sensitive(save));
        g_signal_emit_by_name(compare, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(result)), "Reported minus expected: 0.01"));
      } else
        CHECK(!strcmp(mode, "snapshot"));
    }
  }
  if (signal != 0U)
    g_signal_handler_disconnect(result, signal);
  gtk_window_destroy(parent);
  gtk_window_destroy(window);
  g_signal_emit_by_name(compare, "clicked");
  g_object_unref(compare);
  g_object_unref(result);
  g_object_unref(save);
  g_object_unref(window);
  g_object_unref(parent);
  return 0;
}
