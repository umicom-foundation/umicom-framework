/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bookmark_selection/test_native.c
 *
 * PURPOSE:
 *   Exercise native bookmark rows against stale publications, busy operations and retired owners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/bookmark_panel.h"
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <gtk/gtk.h>
typedef struct Native {
  BookmarkFixture core;
  GtkApplication *app;
  UmiUiApplicationShell *shell;
  UmiGtk4Adapter *adapter;
  GtkWindow *window;
  GtkWidget *panel;
  unsigned calls, destroyed;
  UmiStatus status;
  int visited;
  const char *action;
} Native;
static GtkWidget *Find(GtkWidget *root, const char *tag) {
  const char *found = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
  if (found && !strcmp(found, tag))
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *match = Find(child, tag);
    if (match)
      return match;
  }
  return NULL;
}
static void Result(void *context, UmiStatus status, int visited) {
  Native *f = context;
  ++f->calls;
  f->status = status;
  f->visited = visited;
  if (!strcmp(f->action, "callback-unbind"))
    CHECK(UmiGtk4AdapterBindDocumentEditing(f->adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
  if (!strcmp(f->action, "callback-close"))
    gtk_window_destroy(f->window);
}
static void ContextDestroyed(gpointer context) {
  Native *f = context;
  ++f->destroyed;
}
static void ObserveLabel(GObject *object, GParamSpec *spec, gpointer context) {
  (void)object;
  (void)spec;
  Native *f = context;
  ++f->calls;
  if (!strcmp(f->action, "summary-unbind"))
    CHECK(UmiGtk4AdapterBindDocumentEditing(f->adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
  else
    CHECK(UmiGtk4BookmarkPanelRefresh(f->panel) == UMI_STATUS_BUSY);
}
static void Open(Native *f, const char *action) {
  memset(f, 0, sizeof *f);
  f->action = action;
  BookmarkCreate(&f->core);
  f->app = gtk_application_new("org.umicom.BookmarkPanelTest", G_APPLICATION_NON_UNIQUE);
  CHECK(g_application_register(G_APPLICATION(f->app), NULL, NULL));
  CHECK(umi_ui_application_shell_create("org.umicom.BookmarkPanelTest", "Bookmarks",
                                        f->core.workbench, &f->shell) == UMI_STATUS_OK);
  CHECK(umi_gtk4_adapter_create(f->app, &f->adapter) == UMI_STATUS_OK);
  CHECK(umi_gtk4_adapter_prepare(f->adapter, f->shell) == UMI_STATUS_OK);
  CHECK(UmiGtk4AdapterBindDocumentEditing(f->adapter, f->core.documents, NULL, NULL) ==
        UMI_STATUS_OK);
  f->window = g_object_ref(umi_gtk4_adapter_native_window(f->adapter));
  f->panel = UmiGtk4BookmarkPanelCreate(f->adapter, Result, f, ContextDestroyed);
  CHECK(f->panel && !g_object_is_floating(f->panel));
  GtkWidget *box = gtk_window_get_child(f->window);
  CHECK(GTK_IS_BOX(box));
  gtk_box_append(GTK_BOX(box), f->panel);
}
static void Close(Native *f) {
  (void)UmiGtk4AdapterBindDocumentEditing(f->adapter, NULL, NULL, NULL);
  umi_gtk4_adapter_destroy(f->adapter);
  g_clear_object(&f->panel);
  g_clear_object(&f->window);
  CHECK(f->destroyed == 1U);
  umi_ui_application_shell_destroy(f->shell);
  BookmarkDestroy(&f->core);
  g_clear_object(&f->app);
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  int known = 0;
  const char *cases[] = {"visit",          "remove",         "closed",           "stale",
                         "retained-row",   "unbound",        "retired",          "callback-unbind",
                         "callback-close", "summary-unbind", "refresh-reentry",  "save-busy",
                         "close-busy",     "invalid",        "detached-control", "context-destroy"};
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  Native f;
  Open(&f, name);
  GtkWidget *button =
      Find(f.panel, !strcmp(name, "remove") ? "umicom-bookmark-remove" : "umicom-bookmark-visit");
  CHECK(GTK_IS_BUTTON(button));
  g_object_ref(button);
  if (!strcmp(name, "invalid")) {
    CHECK(UmiGtk4BookmarkPanelCreate(NULL, NULL, NULL, NULL) == NULL);
    CHECK(UmiGtk4BookmarkPanelRefresh(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    GtkWidget *other = gtk_label_new("other");
    g_object_ref_sink(other);
    CHECK(UmiGtk4BookmarkPanelRefresh(other) == UMI_STATUS_INVALID_ARGUMENT);
    g_object_unref(other);
  } else if (!strcmp(name, "closed")) {
    CHECK(UmiDocumentCoordinatorClose(f.core.documents, f.core.active.document_id, 1) ==
          UMI_STATUS_OK);
    CHECK(UmiGtk4BookmarkPanelRefresh(f.panel) == UMI_STATUS_OK);
    GtkWidget *closed = Find(f.panel, "umicom-bookmark-visit");
    CHECK(!gtk_widget_get_sensitive(closed));
    g_signal_emit_by_name(Find(f.panel, "umicom-bookmark-remove"), "clicked");
    CHECK(UmiDocumentCoordinatorBookmarks(f.core.documents, &f.core.marks) == UMI_STATUS_OK &&
          !f.core.marks.count);
  } else if (!strcmp(name, "summary-unbind") || !strcmp(name, "refresh-reentry")) {
    BookmarkCaret(&f.core, 13U);
    CHECK(UmiDocumentCoordinatorToggleBookmark(f.core.documents, NULL) == UMI_STATUS_OK);
    GtkWidget *label = Find(f.panel, "umicom-bookmark-summary");
    CHECK(label);
    gulong signal = g_signal_connect(label, "notify::label", G_CALLBACK(ObserveLabel), &f);
    UmiStatus status = UmiGtk4BookmarkPanelRefresh(f.panel);
    CHECK(f.calls > 0U);
    CHECK(status == (!strcmp(name, "summary-unbind") ? UMI_STATUS_CANCELLED : UMI_STATUS_OK));
    g_signal_handler_disconnect(label, signal);
    if (!strcmp(name, "summary-unbind")) {
      g_signal_emit_by_name(button, "clicked");
      CHECK(BookmarkView(&f.core).cursor_offset == 13U);
    }
  } else if (!strcmp(name, "context-destroy")) {
    CHECK(!f.destroyed);
  } else {
    if (!strcmp(name, "stale")) {
      BookmarkCaret(&f.core, 13U);
      CHECK(UmiDocumentCoordinatorToggleBookmark(f.core.documents, NULL) == UMI_STATUS_OK);
    }
    if (!strcmp(name, "retained-row"))
      CHECK(UmiGtk4BookmarkPanelRefresh(f.panel) == UMI_STATUS_OK);
    if (!strcmp(name, "unbound"))
      CHECK(UmiGtk4AdapterBindDocumentEditing(f.adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
    if (!strcmp(name, "retired"))
      gtk_window_destroy(f.window);
    if (!strcmp(name, "detached-control"))
      gtk_box_remove(GTK_BOX(gtk_widget_get_parent(f.panel)), f.panel);
    if (!strcmp(name, "save-busy"))
      CHECK(UmiGtk4AdapterDocumentSaveAll(f.adapter, NULL, NULL) == UMI_STATUS_OK);
    if (!strcmp(name, "close-busy"))
      CHECK(UmiGtk4AdapterRequestDocumentClose(f.adapter, NULL) == UMI_STATUS_OK);
    g_signal_emit_by_name(button, "clicked");
    UmiUiDocumentViewSnapshot view = BookmarkView(&f.core);
    if (!strcmp(name, "visit") || !strcmp(name, "callback-unbind") ||
        !strcmp(name, "callback-close")) {
      CHECK(view.cursor_offset == 7U && f.calls == 1U && f.status == UMI_STATUS_OK && f.visited);
    } else if (!strcmp(name, "remove")) {
      CHECK(f.core.marks.count == 0U && f.calls == 1U && !f.visited && view.cursor_offset == 0U);
    } else
      CHECK(view.cursor_offset == (!strcmp(name, "stale") ? 13U : 0U));
    CHECK(view.dirty && !strcmp(view.source_text, "first\nsecond\nthird\n"));
  }
  g_object_unref(button);
  Close(&f);
  return 0;
}
