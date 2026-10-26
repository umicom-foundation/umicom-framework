/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_dock.c
 *
 * PURPOSE:
 *   Keep a single board document owner while its existing native contents move between a host pane
 * and a window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/kanban_dock.h"
#include "window_removal_private.h"
#include "focus_handoff_private.h"
#define DOCK_KEY "umicom-kanban-dock"
#define WINDOW_DOCK_KEY "umicom-kanban-window-dock"
typedef struct DockState {
  GtkWidget *root, *content, *status;
  GtkWindow *window;
  bool attached, moving, retired, return_requested;
} DockState;
static void FreeWeak(gpointer data) {
  GWeakRef *weak = data;
  g_weak_ref_clear(weak);
  g_free(weak);
}
static GtkWidget *Current(GtkWindow *window) {
  GWeakRef *weak = g_object_get_data(G_OBJECT(window), WINDOW_DOCK_KEY);
  return weak != NULL ? g_weak_ref_get(weak) : NULL;
}
static bool Live(DockState *state) {
  return !state->retired && UmiGtk4WindowRegistered(state->window) &&
         gtk_window_get_application(state->window) != NULL;
}
/* The board window continues owning all worker callbacks and the document
 * lease. Only the content widget moves, so GTK cannot create two editors of
 * the same mutable board. References cover unparent and notification points. */
static UmiStatus Return(DockState *state, bool present) {
  if (state->moving) {
    state->return_requested = true;
    return UMI_STATUS_BUSY;
  }
  if (!state->attached)
    return state->retired ? UMI_STATUS_CANCELLED : UMI_STATUS_NOT_FOUND;
  state->moving = true;
  state->attached = false;
  g_object_set_data(G_OBJECT(state->window), WINDOW_DOCK_KEY, NULL);
  /* The old native root must release text input before reparenting. */
  UmiGtk4ReleaseSubtreeFocus(state->content);
  if (gtk_widget_get_parent(state->content) == state->root)
    gtk_box_remove(GTK_BOX(state->root), state->content);
  UmiStatus status = UMI_STATUS_CANCELLED;
  if (Live(state) && (gtk_window_get_child(state->window) == state->content ||
                      (gtk_widget_get_parent(state->content) == NULL &&
                       gtk_window_get_child(state->window) == NULL))) {
    if (gtk_window_get_child(state->window) != state->content)
      gtk_window_set_child(state->window, state->content);
    if (Live(state) && gtk_window_get_child(state->window) == state->content) {
      if (present)
        gtk_window_present(state->window);
      status = Live(state) ? UMI_STATUS_OK : UMI_STATUS_CANCELLED;
    }
  }
  state->moving = false;
  return status;
}
static void FreeState(gpointer data) {
  DockState *state = data;
  /* Finalization is also a detach request. The strongly retained content
   * survives GTK's child-disposal ordering until its original owner resumes. */
  (void)Return(state, true);
  g_object_unref(state->content);
  g_object_unref(state->status);
  g_object_unref(state->window);
  g_free(state);
}
UmiStatus UmiKanbanGtk4Undock(GtkWidget *pane, bool present) {
  if (!GTK_IS_WIDGET(pane))
    return UMI_STATUS_INVALID_ARGUMENT;
  DockState *state = g_object_get_data(G_OBJECT(pane), DOCK_KEY);
  if (state == NULL)
    return UMI_STATUS_NOT_FOUND;
  g_object_ref(pane);
  UmiStatus status = Return(state, present);
  if (status == UMI_STATUS_OK)
    gtk_label_set_text(
        GTK_LABEL(state->status),
        "Board returned to its separate window. Dock it again from the project-board controls.");
  g_object_unref(pane);
  return status;
}
static void Click(GtkButton *button, gpointer data) {
  (void)button;
  (void)UmiKanbanGtk4Undock(GTK_WIDGET(data), true);
}
static void HostRemoved(GtkWidget *window, gpointer data) {
  (void)window;
  (void)UmiKanbanGtk4Undock(GTK_WIDGET(data), true);
}
static void BoardRemoved(GtkWidget *window, gpointer data) {
  (void)window;
  GtkWidget *pane = data;
  DockState *state = g_object_get_data(G_OBJECT(pane), DOCK_KEY);
  if (state == NULL)
    return;
  state->retired = true;
  if (!state->moving)
    (void)Return(state, false);
  gtk_widget_set_sensitive(pane, FALSE);
}
bool UmiKanbanGtk4WindowDocked(GtkWindow *board) {
  if (!GTK_IS_WINDOW(board))
    return false;
  GtkWidget *pane = Current(board);
  DockState *state = pane != NULL ? g_object_get_data(G_OBJECT(pane), DOCK_KEY) : NULL;
  bool result = state != NULL && state->attached && Live(state);
  g_clear_object(&pane);
  return result;
}
UmiStatus UmiKanbanGtk4PresentWindow(GtkWindow *board) {
  if (!GTK_IS_WINDOW(board))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!UmiGtk4WindowRegistered(board) || gtk_window_get_application(board) == NULL)
    return UMI_STATUS_CANCELLED;
  if (g_object_get_data(G_OBJECT(board), "umicom-kanban-window") == NULL)
    return UMI_STATUS_NOT_FOUND;
  g_object_ref(board);
  GtkWidget *pane = Current(board);
  UmiStatus status = UMI_STATUS_OK;
  if (pane != NULL)
    status = UmiKanbanGtk4Undock(pane, true);
  else
    gtk_window_present(board);
  if (!UmiGtk4WindowRegistered(board))
    status = UMI_STATUS_CANCELLED;
  g_clear_object(&pane);
  g_object_unref(board);
  return status;
}
UmiStatus UmiKanbanGtk4DockWindow(GtkWindow *host, GtkWindow *board, GtkWidget **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (!GTK_IS_WINDOW(host) || !GTK_IS_WINDOW(board) || host == board)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!UmiGtk4WindowRegistered(host) || !UmiGtk4WindowRegistered(board) ||
      gtk_window_get_application(host) == NULL ||
      gtk_window_get_application(host) != gtk_window_get_application(board))
    return UMI_STATUS_INVALID_STATE;
  if (g_object_get_data(G_OBJECT(board), "umicom-kanban-window") == NULL)
    return UMI_STATUS_NOT_FOUND;
  GtkWidget *prior = Current(board);
  if (prior != NULL) {
    g_object_unref(prior);
    return UMI_STATUS_ALREADY_EXISTS;
  }
  if (UmiKanbanGtk4WindowBusy(board))
    return UMI_STATUS_BUSY;
  GtkWidget *content = gtk_window_get_child(board);
  if (content == NULL)
    return UMI_STATUS_INVALID_STATE;
  DockState *state = g_new0(DockState, 1);
  state->window = g_object_ref(board);
  state->content = g_object_ref(content);
  state->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  g_object_ref_sink(state->root);
  state->status = gtk_label_new("This pane edits the same local board as its independent window.");
  g_object_ref_sink(state->status);
  gtk_label_set_wrap(GTK_LABEL(state->status), TRUE);
  gtk_widget_set_name(state->root, "kanban-docked-board");
  gtk_box_append(GTK_BOX(state->root), state->status);
  GtkWidget *button = gtk_button_new_with_label("Open board in separate window");
  gtk_widget_set_name(button, "kanban-undock");
  g_signal_connect_object(button, "clicked", G_CALLBACK(Click), state->root, 0);
  gtk_box_append(GTK_BOX(state->root), button);
  g_object_set_data_full(G_OBJECT(state->root), DOCK_KEY, state, FreeState);
  UmiGtk4ObserveWindowRemoval(host, G_OBJECT(state->root), HostRemoved, state->root);
  UmiGtk4ObserveWindowRemoval(board, G_OBJECT(state->root), BoardRemoved, state->root);
  GWeakRef *weak = g_new(GWeakRef, 1);
  g_weak_ref_init(weak, G_OBJECT(state->root));
  g_object_set_data_full(G_OBJECT(board), WINDOW_DOCK_KEY, weak, FreeWeak);
  state->attached = true;
  state->moving = true;
  /* Publish the movement guard before focus observers can request a return. */
  UmiGtk4ReleaseSubtreeFocus(content);
  gtk_widget_set_visible(GTK_WIDGET(board), FALSE);
  if (Live(state) && !state->return_requested)
    gtk_window_set_child(board, NULL);
  if (Live(state) && !state->return_requested && gtk_widget_get_parent(content) == NULL)
    gtk_box_append(GTK_BOX(state->root), content);
  state->moving = false;
  if (!Live(state) || state->return_requested || gtk_widget_get_parent(content) != state->root) {
    (void)Return(state, true);
    g_object_unref(state->root);
    return UMI_STATUS_CANCELLED;
  }
  *out = state->root;
  return UMI_STATUS_OK;
}
