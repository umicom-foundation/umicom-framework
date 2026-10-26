/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_gtk4.c
 *
 * PURPOSE:
 *   Exercise native saved-report filters, worker cancellation and retained-control lifetimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#undef CHECK
#include "fixture.h"
#include "umicom/testing/ctest_report_file.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/ctest_report.h"
#include "../support/main_context.h"
static GtkWidget *Find(GtkWidget *root, const char *tag) {
  return umi_gtk4_automation_find_tagged_widget(root, tag);
}
static void Click(GtkWidget *root, const char *tag) {
  GtkWidget *b = Find(root, tag);
  CHECK(GTK_IS_BUTTON(b));
  g_signal_emit_by_name(b, "clicked");
}
/* Bound dispatch so a perpetually ready source cannot hide the fixture deadline.
 * Retain the former idle-draining loop for review. */
#if 0
static void Drain(void) {
  for (unsigned i = 0; i < 40U; ++i) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000U);
  }
}
#endif
static void Drain(void) {
  gint64 end = g_get_monotonic_time() + 40 * G_TIME_SPAN_MILLISECOND;
  while (g_get_monotonic_time() < end) {
    (void)UmiTestDispatchSlice(NULL, end, 32U);
    g_usleep(1000U);
  }
}
static void Wait(GtkWidget *root) {
  gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
  while (UmiGtk4CtestReportBusy(root)) {
    CHECK(g_get_monotonic_time() < end);
    g_main_context_iteration(NULL, FALSE);
    g_usleep(1000U);
  }
}
static char *Text(GtkTextBuffer *buffer) {
  GtkTextIter a, z;
  gtk_text_buffer_get_bounds(buffer, &a, &z);
  return gtk_text_buffer_get_text(buffer, &a, &z, FALSE);
}
static void CloseOnText(GtkTextBuffer *buffer, gpointer window) {
  (void)buffer;
  gtk_window_destroy(GTK_WINDOW(window));
}
/* Accepted publication completes its metadata even if a synchronous observer
 * cancels or hides the panel after the worker result has already been admitted. */
static void CancelOnText(GtkTextBuffer *buffer, GtkWidget *root) {
  (void)buffer;
  Click(root, "ctest.report.cancel");
}
static void HideOnText(GtkTextBuffer *buffer, GtkWidget *root) {
  (void)buffer;
  gtk_widget_set_visible(root, FALSE);
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *names[] = {"normal",       "paging",        "filter",         "draft-filter",
                         "outcomes",     "missing",       "invalid-filter", "hidden",
                         "retained",     "cancel-load",   "hide-show",      "close-load",
                         "close-render", "captured-path", "clear", "captured-filter",
                         "replace-report", "cancel-replacement", "missing-second-page", "invalid-open-filter",
                         "cancel-render", "hide-render", "hide-clear", "cancel-page"};
  CHECK(Known(name, names, sizeof names / sizeof names[0]));
  if (!gtk_init_check())
    return 77;
  char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], missing[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  FixturePath(path, directory, "report.xml");
  FixturePath(missing, directory, "missing.xml");
  char *xml = ManyXml(150);
  UmiOutputFile *file = NULL;
  OK(UmiOutputFileCreate(path, &file));
  OK(UmiOutputFileWrite(file, xml, strlen(xml)));
  OK(UmiOutputFileClose(file));
  UmiOutputFileDestroy(file);
  free(xml);
  GtkApplication *app = gtk_application_new("org.umicom.CtestReport", G_APPLICATION_NON_UNIQUE);
  CHECK(g_application_register(G_APPLICATION(app), NULL, NULL));
  GtkWindow *window = GTK_WINDOW(gtk_application_window_new(app));
  g_object_ref(window);
  GtkWidget *root = UmiGtk4CtestReportCreate();
  g_object_ref_sink(root);
  gtk_window_set_child(window, root);
  gtk_window_present(window);
  Drain();
  GtkWidget *entry = Find(root, "ctest.report.path"), *filter = Find(root, "ctest.report.name"),
            *open = Find(root, "ctest.report.open");
  CHECK(GTK_IS_EDITABLE(entry) && GTK_IS_BUTTON(open) && gtk_widget_get_mapped(open));
  GtkTextBuffer *buffer =
      g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "ctest.report.output"))));
  gtk_editable_set_text(GTK_EDITABLE(entry), path);
  if (!strcmp(name, "hidden")) {
    gtk_widget_set_visible(root, FALSE);
    Click(root, "ctest.report.open");
    CHECK(!UmiGtk4CtestReportBusy(root));
    goto done;
  }
  if (!strcmp(name, "retained")) {
    g_object_ref(open);
    gtk_window_destroy(window);
    g_signal_emit_by_name(open, "clicked");
    CHECK(!UmiGtk4CtestReportBusy(root));
    g_object_unref(open);
    goto done;
  }
  Click(root, "ctest.report.open");
  CHECK(UmiGtk4CtestReportBusy(root));
  if (!strcmp(name, "cancel-load"))
    Click(root, "ctest.report.cancel");
  if (!strcmp(name, "hide-show")) {
    gtk_widget_set_visible(root, FALSE);
    gtk_widget_set_visible(root, TRUE);
  }
  if (!strcmp(name, "close-load"))
    gtk_window_destroy(window);
  if (!strcmp(name, "captured-path"))
    gtk_editable_set_text(GTK_EDITABLE(entry), missing);
  if (!strcmp(name, "captured-filter"))
    gtk_editable_set_text(GTK_EDITABLE(filter), "no-match");
  if (!strcmp(name, "cancel-render"))
    g_signal_connect_object(buffer, "changed", G_CALLBACK(CancelOnText), root, 0);
  if (!strcmp(name, "hide-render"))
    g_signal_connect_object(buffer, "changed", G_CALLBACK(HideOnText), root, 0);
  Wait(root);
  char *text = Text(buffer);
  if (!strcmp(name, "cancel-load") || !strcmp(name, "hide-show") || !strcmp(name, "close-load")) {
    CHECK(text[0] == '\0');
    g_free(text);
    goto done;
  }
  CHECK(strstr(text, "Showing 1..64 of 150"));
  g_free(text);
  if (!strcmp(name, "cancel-render") || !strcmp(name, "hide-render")) {
    CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.report.source"))), path));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.report.status"))),
        "Saved report loaded"));
    if (!strcmp(name, "hide-render")) CHECK(!gtk_widget_get_visible(root));
  }
  if (!strcmp(name, "cancel-page")) {
    g_signal_connect_object(buffer, "changed", G_CALLBACK(CancelOnText), root, 0);
    Click(root, "ctest.report.next");
    text = Text(buffer);
    CHECK(strstr(text, "Showing 65..128 of 150"));
    CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.report.source"))), path));
    CHECK(!strstr(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.report.status"))), "cancelled"));
    g_free(text);
  }
  if (!strcmp(name, "hide-clear")) {
    g_signal_connect_object(buffer, "changed", G_CALLBACK(HideOnText), root, 0);
    Click(root, "ctest.report.clear");
    text = Text(buffer);
    CHECK(!text[0] && !gtk_widget_get_visible(root));
    CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.report.source"))), "No report loaded"));
    CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.report.status"))),
        "Saved report closed; its source file is unchanged."));
    g_free(text);
  }
  if (!strcmp(name, "captured-path"))
    CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.report.source"))), path));
  if (!strcmp(name, "captured-filter")) {
    Click(root, "ctest.report.apply");
    text = Text(buffer);
    CHECK(strstr(text, "Showing 0..0 of 0"));
    g_free(text);
    goto done;
  }
  if (!strcmp(name, "replace-report") || !strcmp(name, "cancel-replacement")) {
    char replacement[UMI_PATH_CAPACITY];
    FixturePath(replacement, directory, "replacement.xml");
    char *new_xml = ManyXml(3);
    UmiOutputFile *replacement_file = NULL;
    OK(UmiOutputFileCreate(replacement, &replacement_file));
    OK(UmiOutputFileWrite(replacement_file, new_xml, strlen(new_xml)));
    OK(UmiOutputFileClose(replacement_file));
    UmiOutputFileDestroy(replacement_file);
    free(new_xml);
    gtk_editable_set_text(GTK_EDITABLE(entry), replacement);
    Click(root, "ctest.report.open");
    if (!strcmp(name, "cancel-replacement")) Click(root, "ctest.report.cancel");
    Wait(root);
    text = Text(buffer);
    CHECK(strstr(text, !strcmp(name, "replace-report") ? "Showing 1..3 of 3" : "Showing 1..64 of 150"));
    CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.report.source"))),
        !strcmp(name, "replace-report") ? replacement : path));
    g_free(text);
    goto done;
  }
  if (!strcmp(name, "missing-second-page")) {
    Click(root, "ctest.report.next");
    char *accepted = Text(buffer);
    CHECK(strstr(accepted, "Showing 65..128 of 150"));
    gtk_editable_set_text(GTK_EDITABLE(entry), missing);
    Click(root, "ctest.report.open");
    Wait(root);
    text = Text(buffer);
    CHECK(!strcmp(text, accepted));
    g_free(text); g_free(accepted);
    goto done;
  }
  if (!strcmp(name, "invalid-open-filter")) {
    /* A populated drop-down may immediately select an item again. Keep the
     * original model alive and remove it to create a real missing selection;
     * this exercises draft refusal without depending on selection policy. */
#if 0
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "ctest.report.outcome")), GTK_INVALID_LIST_POSITION);
#endif
    GtkDropDown *outcome = GTK_DROP_DOWN(Find(root, "ctest.report.outcome"));
    GListModel *outcome_model = gtk_drop_down_get_model(outcome);
    CHECK(outcome_model != NULL);
    g_object_ref(outcome_model);
    gtk_drop_down_set_model(outcome, NULL);
    CHECK(gtk_drop_down_get_selected(outcome) == GTK_INVALID_LIST_POSITION);
    Click(root, "ctest.report.open");
    CHECK(!UmiGtk4CtestReportBusy(root));
    text = Text(buffer);
    CHECK(strstr(text, "Showing 1..64 of 150"));
    g_free(text);
    gtk_drop_down_set_model(outcome, outcome_model);
    g_object_unref(outcome_model);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "ctest.report.outcome")), 0);
    gtk_editable_set_text(GTK_EDITABLE(filter), "row-000007");
    Click(root, "ctest.report.apply");
    text = Text(buffer); CHECK(strstr(text, "Showing 1..1 of 1")); g_free(text);
    goto done;
  }
  if (!strcmp(name, "paging") || !strcmp(name, "draft-filter")) {
    if (!strcmp(name, "draft-filter"))
      gtk_editable_set_text(GTK_EDITABLE(filter), "no-match");
    Click(root, "ctest.report.next");
    text = Text(buffer);
    CHECK(strstr(text, "Showing 65..128 of 150"));
    g_free(text);
    Click(root, "ctest.report.next");
    text = Text(buffer);
    CHECK(strstr(text, "Showing 129..150 of 150"));
    g_free(text);
    Click(root, "ctest.report.previous");
    text = Text(buffer);
    CHECK(strstr(text, "Showing 65..128 of 150"));
    g_free(text);
  } else if (!strcmp(name, "filter")) {
    gtk_editable_set_text(GTK_EDITABLE(filter), "ROW-000007");
    Click(root, "ctest.report.apply");
    text = Text(buffer);
    CHECK(strstr(text, "Showing 1..1 of 1") && strstr(text, "row-000007"));
    g_free(text);
  } else if (!strcmp(name, "outcomes")) {
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "ctest.report.outcome")), 2U);
    Click(root, "ctest.report.apply");
    text = Text(buffer);
    CHECK(strstr(text, "Showing 0..0 of 0"));
    g_free(text);
  } else if (!strcmp(name, "missing")) {
    gtk_editable_set_text(GTK_EDITABLE(entry), missing);
    Click(root, "ctest.report.open");
    Wait(root);
    text = Text(buffer);
    /* A rejected candidate now retains the accepted page; the former
     * clear-on-failure expectation is kept for behavior review. */
#if 0
    CHECK(!text[0]);
#endif
    CHECK(strstr(text, "Showing 1..64 of 150"));
    g_free(text);
  } else if (!strcmp(name, "invalid-filter")) {
    char wide[511];
    for (size_t i = 0; i < 255U; ++i) {
      wide[2 * i] = (char)0xc3;
      wide[2 * i + 1] = (char)0xa9;
    }
    wide[510] = '\0';
    gtk_editable_set_text(GTK_EDITABLE(filter), wide);
    Click(root, "ctest.report.apply");
    text = Text(buffer);
    /* A rejected candidate now retains the accepted page; the former
     * clear-on-failure expectation is kept for behavior review. */
#if 0
    CHECK(!text[0]);
#endif
    CHECK(strstr(text, "Showing 1..64 of 150"));
    g_free(text);
  } else if (!strcmp(name, "close-render")) {
    g_signal_connect_object(buffer, "changed", G_CALLBACK(CloseOnText), window, 0);
    Click(root, "ctest.report.apply");
    CHECK(!gtk_widget_get_mapped(root));
  } else if (!strcmp(name, "clear")) {
    Click(root, "ctest.report.clear");
    text = Text(buffer);
    CHECK(!text[0]);
    g_free(text);
    UmiCtestReport *read = NULL;
    OK(UmiCtestReportReadFile(path, &read));
    UmiCtestReportDestroy(read);
  }
done:
  gtk_window_destroy(window);
  g_object_unref(buffer);
  g_object_unref(window);
  g_object_unref(root);
  g_object_unref(app);
  return 0;
}
