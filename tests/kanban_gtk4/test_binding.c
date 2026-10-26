/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_gtk4/test_binding.c
 *
 * PURPOSE:
 *   Verify explicit remembered locations, async field preservation and retired board windows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/kanban/binding_storage.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/path.h"
#include "umicom/ui/gtk4/kanban_binding.h"
static GtkWidget *Find(GtkWidget *widget, const char *name) {
  if (!strcmp(gtk_widget_get_name(widget), name))
    return widget;
  for (GtkWidget *child = gtk_widget_get_first_child(widget); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = Find(child, name);
    if (found)
      return found;
  }
  return NULL;
}
static void Wait(GtkWindow *window) {
  gint64 deadline = g_get_monotonic_time() + 8 * G_TIME_SPAN_SECOND;
  while (UmiKanbanGtk4WindowBusy(window) && g_get_monotonic_time() < deadline) {
    g_main_context_iteration(NULL, FALSE);
    g_usleep(1000U);
  }
  CHECK(!UmiKanbanGtk4WindowBusy(window));
}
/* Child notifications can reenter a constructor through application observers.
 * Hold the window in the caller and keep any borrowed editable text local to
 * the call; the implementation must own its input before emitting GTK changes. */
typedef struct EnableObserver {
  GtkWindow *window;
  GtkEditable *folder;
  bool retire;
  unsigned fired;
} EnableObserver;
static void EnableChanged(GListModel *model, guint position, guint removed, guint added,
                          gpointer data) {
  (void)model;
  (void)position;
  (void)removed;
  EnableObserver *observer = data;
  if (!added || observer->fired)
    return;
  ++observer->fired;
  if (observer->retire)
    gtk_window_destroy(observer->window);
  else
    gtk_editable_set_text(observer->folder, "observer-replaced-directory");
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  const char *cases[] = {
      "enable",  "repeat",      "generic", "closed",  "remember",           "recall",
      "changed", "retire-load", "missing", "invalid", "borrowed-directory", "enable-retire"};
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = true;
  CHECK(known);
  if (!gtk_init_check())
    return 77;
  char directory[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  GtkApplication *app = gtk_application_new("org.umicom.BindingTest", G_APPLICATION_NON_UNIQUE);
  CHECK(g_application_register(G_APPLICATION(app), NULL, NULL));
  GtkWindow *window = NULL;
  if (!strcmp(name, "generic"))
    window = UmiKanbanGtk4CreateWindow(app);
  else
    CHECK(UmiKanbanGtk4CreateProjectWindow(app, directory, &window) == UMI_STATUS_OK);
  CHECK(window != NULL);
  g_object_ref(window);
  if (!strcmp(name, "borrowed-directory") || !strcmp(name, "enable-retire")) {
    GtkWidget *controls = Find(GTK_WIDGET(window), "kanban-document-controls");
    GtkWidget *entry = Find(GTK_WIDGET(window), "kanban-document-directory");
    CHECK(controls != NULL && GTK_IS_EDITABLE(entry));
    gtk_editable_set_text(GTK_EDITABLE(entry), directory);
    EnableObserver observer = {window, GTK_EDITABLE(entry), !strcmp(name, "enable-retire"), 0U};
    GListModel *children = gtk_widget_observe_children(controls);
    gulong handler =
        g_signal_connect(children, "items-changed", G_CALLBACK(EnableChanged), &observer);
    const char *borrowed = gtk_editable_get_text(GTK_EDITABLE(entry));
    UmiStatus enabled = UmiKanbanGtk4EnableProjectStorage(window, borrowed);
    g_signal_handler_disconnect(children, handler);
    g_object_unref(children);
    CHECK(observer.fired == 1U);
    CHECK(enabled == (observer.retire ? UMI_STATUS_CANCELLED : UMI_STATUS_OK));
    if (!observer.retire) {
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "observer-replaced-directory"));
      GtkWidget *message = Find(GTK_WIDGET(window), "kanban-document-status");
      CHECK(GTK_IS_LABEL(message));
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(message)), directory) != NULL);
    }
    goto done;
  }
  if (!strcmp(name, "closed")) {
    gtk_window_destroy(window);
    CHECK(UmiKanbanGtk4EnableProjectStorage(window, directory) == UMI_STATUS_CANCELLED);
    goto done;
  }
  UmiStatus status = UmiKanbanGtk4EnableProjectStorage(window, directory);
  if (!strcmp(name, "generic")) {
    CHECK(status == UMI_STATUS_NOT_FOUND);
    goto done;
  }
  CHECK(status == UMI_STATUS_OK);
  GtkWidget *remember = Find(GTK_WIDGET(window), "kanban-project-remember");
  GtkWidget *recall = Find(GTK_WIDGET(window), "kanban-project-recall");
  GtkWidget *folder = Find(GTK_WIDGET(window), "kanban-document-directory");
  GtkWidget *file = Find(GTK_WIDGET(window), "kanban-document-filename");
  CHECK(GTK_IS_BUTTON(remember) && GTK_IS_BUTTON(recall) && GTK_IS_EDITABLE(folder) &&
        GTK_IS_EDITABLE(file));
  if (!strcmp(name, "repeat")) {
    CHECK(UmiKanbanGtk4EnableProjectStorage(window, directory) == UMI_STATUS_ALREADY_EXISTS);
    goto done;
  }
  if (!strcmp(name, "enable"))
    goto done;
  UmiKanbanProjectBinding binding;
  CHECK(UmiKanbanProjectBindingCreate(directory, directory, "chosen.umikb", &binding) ==
        UMI_STATUS_OK);
  if (!strcmp(name, "remember") || !strcmp(name, "invalid")) {
    gtk_editable_set_text(GTK_EDITABLE(folder), !strcmp(name, "invalid") ? "relative" : directory);
    gtk_editable_set_text(GTK_EDITABLE(file), "chosen.umikb");
    g_signal_emit_by_name(remember, "clicked");
    Wait(window);
    UmiKanbanProjectBinding saved;
    if (!strcmp(name, "invalid")) {
      CHECK(UmiKanbanBindingStorageLoad(directory, directory, &saved) == UMI_STATUS_NOT_FOUND);
    } else {
      CHECK(UmiKanbanBindingStorageLoad(directory, directory, &saved) == UMI_STATUS_OK);
      /* Recall returns the canonical platform path; the stored filename stays exact. */
#if 0
      CHECK(!strcmp(saved.directory, directory) && !strcmp(saved.filename, "chosen.umikb"));
#endif
      CHECK(umi_path_equal(saved.directory, directory) && !strcmp(saved.filename, "chosen.umikb"));
    }
  } else {
    if (strcmp(name, "missing"))
      CHECK(UmiKanbanBindingStorageSave(directory, &binding) == UMI_STATUS_OK);
    char original[UMI_KANBAN_FILENAME_CAPACITY];
    strcpy(original, gtk_editable_get_text(GTK_EDITABLE(file)));
    g_signal_emit_by_name(recall, "clicked");
    if (!strcmp(name, "changed"))
      gtk_editable_set_text(GTK_EDITABLE(file), "my-edit.umikb");
    if (!strcmp(name, "retire-load"))
      gtk_window_destroy(window);
    Wait(window);
    if (!strcmp(name, "changed")) {
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(file)), "my-edit.umikb"));
      CHECK(gtk_editable_get_text(GTK_EDITABLE(folder))[0] == '\0');
    } else if (!strcmp(name, "missing")) {
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(file)), original));
      CHECK(gtk_editable_get_text(GTK_EDITABLE(folder))[0] == '\0');
    } else if (!strcmp(name, "recall")) {
      /* Recall returns the canonical platform path; the stored filename stays exact. */
#if 0
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(folder)), directory));
#endif
      CHECK(umi_path_equal(gtk_editable_get_text(GTK_EDITABLE(folder)), directory));
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(file)), "chosen.umikb"));
    }
  }
  /* Remember/recall must never create the selected board archive. */
  int exists = 1;
  CHECK(UmiRootedFileExists(directory, "chosen.umikb", &exists) == UMI_STATUS_OK && !exists);
done:
  gtk_window_destroy(window);
  g_object_unref(window);
  g_object_unref(app);
  return 0;
}
