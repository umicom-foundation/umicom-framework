/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_dock/test_native.c
 *
 * PURPOSE:
 *   Cover board transfer, reentrant notifications and retained-widget lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static void RemovedDuringHide(GObject *object, GParamSpec *spec, gpointer data) {
  (void)spec;
  if (!gtk_widget_get_visible(GTK_WIDGET(object)))
    gtk_window_destroy(GTK_WINDOW(data));
}
static void ReturnDuringHide(GObject *object, GParamSpec *spec, gpointer data) {
  (void)spec;
  int *calls = data;
  if (!gtk_widget_get_visible(GTK_WIDGET(object))) {
    ++*calls;
    CHECK(UmiKanbanGtk4PresentWindow(GTK_WINDOW(object)) == UMI_STATUS_BUSY);
  }
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  if (!gtk_init_check())
    return 77;
  Fixture f = {0};
  Open(&f);
  if (!strcmp(argv[1], "null")) {
    GtkWidget *out = (GtkWidget *)1;
    CHECK(UmiKanbanGtk4DockWindow(NULL, f.board, &out) == UMI_STATUS_INVALID_ARGUMENT &&
          out == NULL);
    CHECK(UmiKanbanGtk4DockWindow(f.host, f.board, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiKanbanGtk4Undock(NULL, false) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!UmiKanbanGtk4WindowDocked(NULL));
  } else if (!strcmp(argv[1], "same-window")) {
    CHECK(UmiKanbanGtk4DockWindow(f.board, f.board, &f.pane) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(argv[1], "ordinary-window")) {
    CHECK(UmiKanbanGtk4DockWindow(f.board, f.host, &f.pane) == UMI_STATUS_NOT_FOUND);
  } else if (!strcmp(argv[1], "removed-host")) {
    gtk_window_destroy(f.host);
    CHECK(UmiKanbanGtk4DockWindow(f.host, f.board, &f.pane) == UMI_STATUS_INVALID_STATE);
  } else if (!strcmp(argv[1], "different-application")) {
    GtkApplication *other = gtk_application_new("org.umicom.OtherDock", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(other), NULL, NULL));
    GtkWindow *host = GTK_WINDOW(gtk_application_window_new(other));
    CHECK(UmiKanbanGtk4DockWindow(host, f.board, &f.pane) == UMI_STATUS_INVALID_STATE);
    gtk_window_destroy(host);
    g_object_unref(other);
  } else if (!strcmp(argv[1], "host-removal-during-dock")) {
    gtk_widget_set_visible(GTK_WIDGET(f.board), TRUE);
    gulong signal =
        g_signal_connect(f.board, "notify::visible", G_CALLBACK(RemovedDuringHide), f.host);
    CHECK(UmiKanbanGtk4DockWindow(f.host, f.board, &f.pane) == UMI_STATUS_CANCELLED &&
          f.pane == NULL);
    CHECK(gtk_window_get_child(f.board) == f.content);
    g_signal_handler_disconnect(f.board, signal);
  } else if (!strcmp(argv[1], "return-during-dock")) {
    int calls = 0;
    gtk_widget_set_visible(GTK_WIDGET(f.board), TRUE);
    gulong signal =
        g_signal_connect(f.board, "notify::visible", G_CALLBACK(ReturnDuringHide), &calls);
    CHECK(UmiKanbanGtk4DockWindow(f.host, f.board, &f.pane) == UMI_STATUS_CANCELLED && calls == 1);
    CHECK(gtk_window_get_child(f.board) == f.content &&
          gtk_widget_get_visible(GTK_WIDGET(f.board)));
    g_signal_handler_disconnect(f.board, signal);
  } else {
    Dock(&f);
    CHECK(UmiKanbanGtk4WindowDocked(f.board) && gtk_window_get_child(f.board) == NULL);
    CHECK(gtk_widget_get_parent(f.content) == f.pane);
    if (!strcmp(argv[1], "normal")) {
    } else if (!strcmp(argv[1], "duplicate")) {
      GtkWidget *second = NULL;
      CHECK(UmiKanbanGtk4DockWindow(f.host, f.board, &second) == UMI_STATUS_ALREADY_EXISTS &&
            second == NULL);
    } else if (!strcmp(argv[1], "return")) {
      OK(UmiKanbanGtk4Undock(f.pane, true));
      CHECK(gtk_window_get_child(f.board) == f.content);
      CHECK(UmiKanbanGtk4Undock(f.pane, true) == UMI_STATUS_NOT_FOUND);
    } else if (!strcmp(argv[1], "present")) {
      OK(UmiKanbanGtk4PresentWindow(f.board));
      CHECK(!UmiKanbanGtk4WindowDocked(f.board) && gtk_window_get_child(f.board) == f.content);
    } else if (!strcmp(argv[1], "host-removed")) {
      gtk_window_destroy(f.host);
      CHECK(!UmiKanbanGtk4WindowDocked(f.board) && gtk_window_get_child(f.board) == f.content);
      CHECK(gtk_widget_get_visible(GTK_WIDGET(f.board)));
    } else if (!strcmp(argv[1], "board-removed")) {
      gtk_window_destroy(f.board);
      CHECK(!UmiKanbanGtk4WindowDocked(f.board));
      CHECK(UmiKanbanGtk4Undock(f.pane, true) == UMI_STATUS_CANCELLED);
    } else if (!strcmp(argv[1], "pane-finalized")) {
      gtk_window_set_child(f.host, NULL);
      g_clear_object(&f.pane);
      CHECK(gtk_window_get_child(f.board) == f.content &&
            gtk_widget_get_visible(GTK_WIDGET(f.board)));
    } else if (!strcmp(argv[1], "hide-host")) {
      gtk_widget_set_visible(GTK_WIDGET(f.host), TRUE);
      gtk_widget_set_visible(GTK_WIDGET(f.host), FALSE);
      CHECK(UmiKanbanGtk4WindowDocked(f.board));
    } else if (!strcmp(argv[1], "redock")) {
      OK(UmiKanbanGtk4Undock(f.pane, false));
      gtk_window_set_child(f.host, NULL);
      g_clear_object(&f.pane);
      Dock(&f);
      CHECK(gtk_widget_get_parent(f.content) == f.pane);
    } else if (!strcmp(argv[1], "retained-control")) {
      GtkWidget *button = g_object_ref(Find(f.pane, "kanban-undock"));
      gtk_window_destroy(f.host);
      g_clear_object(&f.pane);
      g_signal_emit_by_name(button, "clicked");
      CHECK(gtk_window_get_child(f.board) == f.content);
      g_object_unref(button);
    } else
      return 2;
  }
  Close(&f);
  return 0;
}
