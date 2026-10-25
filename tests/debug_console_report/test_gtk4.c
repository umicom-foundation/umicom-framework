/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_console_report/test_gtk4.c
 *
 * PURPOSE:
 *   Check stable native pages, draft filter semantics, refresh failures and retired controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/debug_console.h"
typedef struct Host {
  UmiDebugConsoleEntryRegistry *registry;
  GtkWindow *window;
  unsigned queries, destroyed;
  int fail, close;
} Host;
static UmiStatus Query(void *context, const UmiDebugConsoleFilter *filter,
                       UmiDebugConsoleReport **out) {
  Host *host = context;
  ++host->queries;
  *out = NULL;
  if (host->fail)
    return UMI_STATUS_PERMISSION_DENIED;
  *out = Capture(host->registry, filter);
  if (host->close)
    gtk_window_destroy(host->window);
  return UMI_STATUS_OK;
}
static void Destroy(gpointer context) { ++((Host *)context)->destroyed; }
static void CloseBuffer(GtkTextBuffer *buffer, gpointer context) {
  (void)buffer;
  gtk_window_destroy(GTK_WINDOW(context));
}
static GtkWidget *Find(GtkWidget *root, const char *tag) {
  return umi_gtk4_automation_find_tagged_widget(root, tag);
}
static void Click(GtkWidget *button) { g_signal_emit_by_name(button, "clicked"); }
static char *ReadText(GtkWidget *root) {
  GtkTextBuffer *b = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "debug.console.output")));
  GtkTextIter a, z;
  gtk_text_buffer_get_bounds(b, &a, &z);
  return gtk_text_buffer_get_text(b, &a, &z, FALSE);
}
static void Drain(void) {
  for (unsigned i = 0; i < 30U; ++i) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000U);
  }
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *names[] = {"normal",      "paging",         "draft-filter", "filter",
                         "failure",     "invalid-filter", "hidden",       "retained",
                         "close-query", "close-render",   "clear",        "hidden-copy"};
  CHECK(Known(name, names, sizeof names / sizeof names[0]));
  if (!gtk_init_check())
    return 77;
  Host host = {0};
  host.registry = Entries(150U);
  GtkApplication *app =
      gtk_application_new("org.umicom.ConsoleReportTest", G_APPLICATION_NON_UNIQUE);
  CHECK(g_application_register(G_APPLICATION(app), NULL, NULL));
  host.window = GTK_WINDOW(gtk_application_window_new(app));
  g_object_ref(host.window);
  GtkWidget *root = UmiGtk4DebugConsoleCreate(Query, &host, Destroy);
  CHECK(root);
  g_object_ref_sink(root);
  gtk_window_set_child(host.window, root);
  gtk_window_present(host.window);
  Drain();
  GtkWidget *refresh = Find(root, "debug.console.refresh"),
            *next = Find(root, "debug.console.next"),
            *previous = Find(root, "debug.console.previous"),
            *filter = Find(root, "debug.console.filter");
  CHECK(GTK_IS_BUTTON(refresh) && gtk_widget_get_mapped(refresh));
  if (!strcmp(name, "hidden")) {
    gtk_widget_set_visible(root, FALSE);
    Click(refresh);
    CHECK(host.queries == 0U);
    goto done;
  }
  if (!strcmp(name, "retained")) {
    g_object_ref(refresh);
    gtk_window_destroy(host.window);
    Click(refresh);
    CHECK(host.queries == 0U);
    g_object_unref(refresh);
    goto done;
  }
  if (!strcmp(name, "close-query"))
    host.close = 1;
  if (!strcmp(name, "close-render")) {
    GtkTextBuffer *b = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "debug.console.output")));
    g_signal_connect_object(b, "changed", G_CALLBACK(CloseBuffer), host.window, 0);
  }
  Click(refresh);
  CHECK(host.queries == 1U);
  if (host.close || !strcmp(name, "close-render"))
    goto done;
  char *text = ReadText(root);
  CHECK(strstr(text, "Showing 1..64 of 150"));
  g_free(text);
  if (!strcmp(name, "paging") || !strcmp(name, "draft-filter")) {
    if (!strcmp(name, "draft-filter"))
      gtk_editable_set_text(GTK_EDITABLE(filter), "no-match");
    umi_debug_console_entry_registry_clear(host.registry);
    Click(next);
    text = ReadText(root);
    CHECK(strstr(text, "Showing 65..128 of 150"));
    g_free(text);
    Click(next);
    text = ReadText(root);
    CHECK(strstr(text, "Showing 129..150"));
    g_free(text);
    Click(next);
    text = ReadText(root);
    CHECK(strstr(text, "Showing 129..150"));
    g_free(text);
    Click(previous);
    text = ReadText(root);
    CHECK(strstr(text, "Showing 65..128"));
    g_free(text);
    CHECK(host.queries == 1U);
  } else if (!strcmp(name, "filter")) {
    gtk_editable_set_text(GTK_EDITABLE(filter), "message-0007");
    Click(refresh);
    text = ReadText(root);
    CHECK(strstr(text, "Showing 1..1 of 1") && strstr(text, "message-0007"));
    g_free(text);
  } else if (!strcmp(name, "failure") || !strcmp(name, "invalid-filter")) {
    if (!strcmp(name, "failure"))
      host.fail = 1;
    else {
      char wide[127];
      for (size_t i = 0; i < 63U; ++i) {
        wide[i * 2U] = (char)0xc3;
        wide[i * 2U + 1U] = (char)0xa9;
      }
      wide[126] = '\0';
      gtk_editable_set_text(GTK_EDITABLE(Find(root, "debug.console.category")), wide);
    }
    Click(refresh);
    text = ReadText(root);
    CHECK(!text[0]);
    g_free(text);
    CHECK(host.queries == (!strcmp(name, "failure") ? 2U : 1U));
  } else if (!strcmp(name, "clear")) {
    Click(Find(root, "debug.console.clear"));
    text = ReadText(root);
    CHECK(!text[0]);
    g_free(text);
    CHECK(umi_debug_console_entry_registry_count(host.registry) == 150U && host.queries == 1U);
    Click(refresh);
    text = ReadText(root);
    CHECK(strstr(text, "of 150"));
    g_free(text);
  } else if (!strcmp(name, "hidden-copy")) {
    GtkWidget *label = Find(root, "debug.console.status");
    char *before = g_strdup(gtk_label_get_text(GTK_LABEL(label)));
    gtk_widget_set_visible(root, FALSE);
    Click(Find(root, "debug.console.copy"));
    CHECK(!strcmp(before, gtk_label_get_text(GTK_LABEL(label))));
    g_free(before);
  }
done:
  gtk_window_destroy(host.window);
  g_object_unref(host.window);
  g_object_unref(root);
  CHECK(host.destroyed == 1U);
  g_object_unref(app);
  umi_debug_console_entry_registry_destroy(host.registry);
  return 0;
}
