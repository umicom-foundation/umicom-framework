/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_gtk4/test_focus_handoff.c
 *
 * PURPOSE:
 *   Verify input focus and model ownership across native board state changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/kanban.h"
#include "../support/main_context.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do {   if (!(condition)) {     fprintf(stderr, "%s:%d: %s
", __FILE__, __LINE__, #condition);     exit(1);   } } while (0)

typedef struct Reentry {
  GtkWidget *panel;
  unsigned calls;
} Reentry;

static GtkWidget *Find(GtkWidget *root, const char *name)
{
  if (strcmp(gtk_widget_get_name(root), name) == 0)
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = Find(child, name);
    if (found != NULL)
      return found;
  }
  return NULL;
}
static void RetireOnFocusLoss(GObject *object, GParamSpec *spec, gpointer data)
{
  (void)spec;
  Reentry *entry = data;
  if (!gtk_widget_has_focus(GTK_WIDGET(object)) && entry->calls == 0U) {
    ++entry->calls;
    UmiKanbanGtk4Retire(entry->panel);
  }
}
int main(int argc, char **argv)
{
  CHECK(argc == 2);
  const char *mode = argv[1];
  bool busy = strcmp(mode, "busy") == 0;
  bool retire = strcmp(mode, "retire") == 0;
  bool reenter = strcmp(mode, "reentrant-retire") == 0;
  bool unrelated = strcmp(mode, "unrelated") == 0;
  bool unrooted = strcmp(mode, "unrooted") == 0;
  CHECK(busy || retire || reenter || unrelated || unrooted);
  if (!gtk_init_check())
    return 77;
  UmiKanbanBoard *board = NULL;
  CHECK(umi_kanban_board_create_workflow("focus", "Focus board", &board) == UMI_STATUS_OK);
  GtkWidget *panel = UmiKanbanGtk4Create(board);
  CHECK(panel != NULL);
  g_object_ref_sink(panel);
  uint64_t revision = umi_kanban_board_revision(board);
  if (unrooted) {
    UmiKanbanGtk4SetBusy(panel, true);
    CHECK(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_UNAVAILABLE);
    UmiKanbanGtk4SetBusy(panel, false);
    CHECK(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
  } else {
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    g_object_ref(window);
    GtkWidget *layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *outside = gtk_entry_new();
    gtk_box_append(GTK_BOX(layout), panel);
    gtk_box_append(GTK_BOX(layout), outside);
    gtk_window_set_child(window, layout);
    gtk_window_present(window);
    (void)UmiTestDispatchSlice(NULL, g_get_monotonic_time() + 50000, 32U);
    GtkWidget *target = unrelated ? outside : Find(panel, "kanban-title");
    CHECK(GTK_IS_ENTRY(target));
    CHECK(gtk_widget_grab_focus(target));
    GtkWidget *focus = gtk_root_get_focus(GTK_ROOT(window));
    CHECK(focus != NULL && (focus == target || gtk_widget_is_ancestor(focus, target)));
    g_object_ref(focus);
    Reentry entry = {panel, 0U};
    gulong signal = reenter
        ? g_signal_connect(focus, "notify::has-focus", G_CALLBACK(RetireOnFocusLoss), &entry)
        : 0U;
    if (retire)
      UmiKanbanGtk4Retire(panel);
    else
      UmiKanbanGtk4SetBusy(panel, true);
    if (signal != 0U)
      g_signal_handler_disconnect(focus, signal);
    CHECK(gtk_root_get_focus(GTK_ROOT(window)) == (unrelated ? focus : NULL));
    CHECK(!gtk_widget_get_sensitive(panel));
    CHECK(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_UNAVAILABLE);
    if (reenter)
      CHECK(entry.calls == 1U);
    UmiKanbanGtk4SetBusy(panel, false);
    CHECK(UmiKanbanGtk4Refresh(panel) ==
          ((retire || reenter) ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK));
    if (retire || reenter)
      CHECK(!gtk_widget_get_sensitive(panel));
    UmiKanbanGtk4Retire(panel);
    gtk_window_set_focus(window, NULL);
    gtk_window_destroy(window);
    g_object_unref(focus);
    g_object_unref(window);
  }
  CHECK(umi_kanban_board_revision(board) == revision);
  UmiKanbanGtk4Retire(panel);
  g_object_unref(panel);
  umi_kanban_board_destroy(board);
  return 0;
}
