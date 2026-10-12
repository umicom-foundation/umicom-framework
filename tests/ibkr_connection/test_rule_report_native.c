/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_rule_report_native.c
 * PURPOSE: Exercise report import and retained native controls using local fixtures only.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "rule_report_fixture.h"
#include "umicom/broker_connectivity/connection_gtk4.h"
#include "umicom/broker_connectivity/rule_report_gtk4.h"
#include "umicom/ui/gtk4/automation.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(expression)                                                                          \
  do {                                                                                             \
    if (!(expression)) {                                                                           \
      fprintf(stderr, "%d: %s\n", __LINE__, #expression);                                          \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name) {
  return umi_gtk4_automation_find_named_widget(root, name);
}
/* Wait for actual worker completion by observing its publication, with a hard
 * timeout. The test never logs in, sends a broker request or starts a trade. */
static gboolean WaitText(GtkWidget *label, const char *text) {
  gint64 until = g_get_monotonic_time() + 5000000;
  do {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    if (strstr(gtk_label_get_text(GTK_LABEL(label)), text))
      return TRUE;
    g_usleep(1000U);
  } while (g_get_monotonic_time() < until);
  return FALSE;
}
static void PanelGone(gpointer data, GObject *object) {
  (void)object;
  *(gboolean *)data = TRUE;
}
typedef struct Reentry {
  GtkWidget *panel;
  GtkWindow *window;
  guint calls;
  gboolean close;
} Reentry;
static void OnComparison(GObject *object, GParamSpec *property, gpointer data) {
  (void)object;
  (void)property;
  Reentry *probe = data;
  ++probe->calls;
  if (probe->close)
    gtk_window_destroy(probe->window);
  else
    UmiIbkrRuleReportGtk4ClearExpectation(probe->panel);
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  if (!gtk_init_check())
    return 77;
  const char *mode = argv[1];
  GtkWindow *window = GTK_WINDOW(gtk_window_new());
  g_object_ref(window);
  GtkWidget *panel = UmiIbkrRuleReportGtk4Create();
  gtk_window_set_child(window, panel);
  g_object_ref(panel);
  GtkWidget *path = Find(panel, "ibkr-rule-report-path");
  GtkWidget *load = Find(panel, "ibkr-rule-report-load");
  GtkWidget *clear = Find(panel, "ibkr-rule-report-clear");
  GtkWidget *output = Find(panel, "ibkr-rule-report-output");
  GtkWidget *comparison = Find(panel, "ibkr-rule-report-comparison");
  CHECK(path && load && clear && output && comparison);
  g_object_ref(load);
  g_object_ref(clear);
  g_object_ref(output);
  g_object_ref(comparison);
  if (!strcmp(mode, "fields")) {
    CHECK(Find(panel, "ibkr-rule-report-routes"));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "No broker"));
  } else if (!strcmp(mode, "invalid-expectation")) {
    CHECK(UmiIbkrRuleReportGtk4SetExpectation(panel, 0U, UMI_SIDE_BUY, "SMART") ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiIbkrRuleReportGtk4SetExpectation(panel, 123U, UMI_SIDE_BUY, " SMART") ==
          UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(mode, "retained")) {
    gchar *before = g_strdup(gtk_label_get_text(GTK_LABEL(output)));
    gtk_window_destroy(window);
    g_signal_emit_by_name(load, "clicked");
    g_signal_emit_by_name(clear, "clicked");
    CHECK(UmiIbkrRuleReportGtk4SetExpectation(panel, 123U, UMI_SIDE_BUY, "SMART") ==
          UMI_STATUS_INVALID_STATE);
    CHECK(!strcmp(before, gtk_label_get_text(GTK_LABEL(output))));
    g_free(before);
  } else if (!strcmp(mode, "policy-integration")) {
    GtkWindow *broker = UmiIbkrGtkCreate(NULL);
    CHECK(broker);
    g_object_ref(broker);
    GtkWidget *embedded = Find(GTK_WIDGET(broker), "ibkr-rule-report");
    GtkWidget *review = Find(GTK_WIDGET(broker), "ibkr-fill-review");
    GtkWidget *price = Find(GTK_WIDGET(broker), "ibkr-fill-price");
    CHECK(embedded && review && price);
    CHECK(UmiIbkrRuleReportGtk4SetExpectation(embedded, 123U, UMI_SIDE_BUY, "SMART") ==
          UMI_STATUS_OK);
    gtk_editable_set_text(GTK_EDITABLE(price), "4.25");
    g_signal_emit_by_name(review, "clicked");
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(embedded, "ibkr-rule-report-comparison"))),
                 "No captured"));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(GTK_WIDGET(broker), "ibkr-status"))),
                 "Not connected"));
    gtk_window_destroy(broker);
    g_object_unref(broker);
  } else {
    GError *error = NULL;
    gchar *directory = g_dir_make_tmp("umicom-rule-report-XXXXXX", &error);
    CHECK(directory && error == NULL);
    gchar *filename = g_build_filename(directory, "rules.json", NULL);
    CHECK(g_file_set_contents(filename, !strcmp(mode, "invalid-file") ? "{" : RULE_REPORT_FIXTURE,
                              -1, &error));
    gtk_editable_set_text(GTK_EDITABLE(path), filename);
    CHECK(UmiIbkrRuleReportGtk4SetExpectation(panel, 123U, UMI_SIDE_BUY, "SMART") == UMI_STATUS_OK);
    g_signal_emit_by_name(load, "clicked");
    if (!strcmp(mode, "path-change")) {
      gtk_editable_set_text(GTK_EDITABLE(path), "changed-before-publication.json");
      /* A second load is ignored while the first worker is pending.
       * Closing discards its result; no old route may be published now. */
      CHECK(!strstr(gtk_label_get_text(GTK_LABEL(output)), "FOK advertised"));
    } else if (!strcmp(mode, "close-loading")) {
      gtk_window_destroy(window);
      gchar *before = g_strdup(gtk_label_get_text(GTK_LABEL(output)));
      g_signal_emit_by_name(load, "clicked");
      CHECK(!strcmp(before, gtk_label_get_text(GTK_LABEL(output))));
      g_free(before);
    } else if (!strcmp(mode, "invalid-file")) {
      CHECK(WaitText(output, "Report not imported"));
      CHECK(!strstr(gtk_label_get_text(GTK_LABEL(comparison)), "match"));
    } else {
      CHECK(WaitText(output, "FOK advertised"));
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(comparison)), "match"));
      if (!strcmp(mode, "clear")) {
        g_signal_emit_by_name(clear, "clicked");
        CHECK(!strstr(gtk_label_get_text(GTK_LABEL(output)), "FOK advertised"));
      } else if (!strcmp(mode, "mismatch")) {
        CHECK(UmiIbkrRuleReportGtk4SetExpectation(panel, 123U, UMI_SIDE_SELL, "SMART") ==
              UMI_STATUS_OK);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(comparison)), "Different side"));
      } else if (!strcmp(mode, "reentrant") || !strcmp(mode, "close-notify")) {
        Reentry probe = {panel, window, 0U, !strcmp(mode, "close-notify")};
        g_signal_connect(comparison, "notify::label", G_CALLBACK(OnComparison), &probe);
        CHECK(UmiIbkrRuleReportGtk4SetExpectation(panel, 124U, UMI_SIDE_BUY, "SMART") ==
              UMI_STATUS_OK);
        CHECK(probe.calls > 0U && probe.calls < 4U);
        if (!probe.close)
          CHECK(!strstr(gtk_label_get_text(GTK_LABEL(comparison)), "match"));
        g_signal_handlers_disconnect_by_data(comparison, &probe);
      } else
        CHECK(!strcmp(mode, "import"));
    }
    /* The source object is held by the task until its completion callback.
     * Teardown may precede that callback; the retained controls stay inert. */
    gtk_window_destroy(window);
    g_object_unref(window);
    window = NULL;
    gboolean gone = FALSE;
    g_object_weak_ref(G_OBJECT(panel), PanelGone, &gone);
    g_object_unref(panel);
    panel = NULL;
    gint64 until = g_get_monotonic_time() + 5000000;
    while (!gone && g_get_monotonic_time() < until) {
      while (g_main_context_iteration(NULL, FALSE)) {
      }
      g_usleep(1000U);
    }
    CHECK(gone);
    if (!strcmp(mode, "path-change") || !strcmp(mode, "close-loading"))
      CHECK(!strstr(gtk_label_get_text(GTK_LABEL(output)), "FOK advertised"));
    CHECK(g_remove(filename) == 0);
    CHECK(g_rmdir(directory) == 0);
    g_free(filename);
    g_free(directory);
    g_clear_error(&error);
  }
  if (window != NULL)
    gtk_window_destroy(window);
  if (panel)
    g_object_unref(panel);
  if (window != NULL)
    g_object_unref(window);
  g_object_unref(load);
  g_object_unref(clear);
  g_object_unref(output);
  g_object_unref(comparison);
  return 0;
}
