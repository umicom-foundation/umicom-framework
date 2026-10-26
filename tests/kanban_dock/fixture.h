/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_dock/fixture.h
 *
 * PURPOSE:
 *   Exercise real native board ownership without a second model or document lease.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KANBAN_DOCK_FIXTURE_H
#define UMICOM_KANBAN_DOCK_FIXTURE_H
#include "../support/main_context.h"
#include "umicom/kanban/document.h"
#include "umicom/ui/gtk4/kanban_dock.h"
#include "umicom/ui/gtk4/kanban_project.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
typedef struct Fixture {
  GtkApplication *app;
  GtkWindow *host, *board;
  GtkWidget *pane, *content;
} Fixture;
static inline GtkWidget *Find(GtkWidget *root, const char *name) {
  if (!strcmp(gtk_widget_get_name(root), name))
    return root;
  for (GtkWidget *c = gtk_widget_get_first_child(root); c; c = gtk_widget_get_next_sibling(c)) {
    GtkWidget *f = Find(c, name);
    if (f)
      return f;
  }
  return NULL;
}
static inline GtkWidget *Button(GtkWidget *root, const char *label) {
  if (GTK_IS_BUTTON(root) && g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), label) == 0)
    return root;
  for (GtkWidget *c = gtk_widget_get_first_child(root); c; c = gtk_widget_get_next_sibling(c)) {
    GtkWidget *f = Button(c, label);
    if (f)
      return f;
  }
  return NULL;
}
static inline void Click(GtkWidget *root, const char *label) {
  GtkWidget *b = Button(root, label);
  CHECK(GTK_IS_BUTTON(b));
  g_signal_emit_by_name(b, "clicked");
}
/* The former unbounded drain is retained for review. A busy native event
 * source must not prevent this fixture from checking worker completion. */
#if 0
static inline void Wait(Fixture *f) {
  gint64 end = g_get_monotonic_time() + 20 * G_TIME_SPAN_SECOND;
  while (UmiKanbanGtk4WindowBusy(f->board) && g_get_monotonic_time() < end) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000);
  }
  CHECK(!UmiKanbanGtk4WindowBusy(f->board));
}
#endif
static inline void Wait(Fixture *f) {
  gint64 end = g_get_monotonic_time() + 20 * G_TIME_SPAN_SECOND;
  while (UmiKanbanGtk4WindowBusy(f->board) && g_get_monotonic_time() < end) {
    (void)UmiTestDispatchSlice(NULL, end, 32U);
    g_usleep(1000);
  }
  CHECK(!UmiKanbanGtk4WindowBusy(f->board));
}
static inline void Open(Fixture *f) {
  f->app = gtk_application_new("org.umicom.KanbanDockTest", G_APPLICATION_NON_UNIQUE);
  CHECK(g_application_register(G_APPLICATION(f->app), NULL, NULL));
  f->host = GTK_WINDOW(gtk_application_window_new(f->app));
  g_object_ref(f->host);
  f->board = UmiKanbanGtk4CreateWindow(f->app);
  CHECK(f->board != NULL);
  g_object_ref(f->board);
  f->content = g_object_ref(gtk_window_get_child(f->board));
}
static inline void Dock(Fixture *f) {
  OK(UmiKanbanGtk4DockWindow(f->host, f->board, &f->pane));
  gtk_window_set_child(f->host, f->pane);
}
static inline void Close(Fixture *f) {
  gtk_window_destroy(f->host);
  if (f->pane != NULL) {
    (void)UmiKanbanGtk4Undock(f->pane, false);
    g_clear_object(&f->pane);
  }
  gtk_window_destroy(f->board);
  Wait(f);
  g_object_unref(f->content);
  g_object_unref(f->host);
  g_object_unref(f->board);
  g_object_unref(f->app);
}
static inline void Create(Fixture *f, const char *directory) {
  gtk_editable_set_text(GTK_EDITABLE(Find(f->content, "kanban-document-directory")), directory);
  gtk_editable_set_text(GTK_EDITABLE(Find(f->content, "kanban-document-filename")), "board.umikb");
  Click(f->content, "Create board");
  Wait(f);
  CHECK(Find(f->content, "kanban-title") != NULL);
}
static inline void Add(Fixture *f, const char *title) {
  gtk_editable_set_text(GTK_EDITABLE(Find(f->content, "kanban-title")), title);
  Click(f->content, "Add card");
}
#endif
