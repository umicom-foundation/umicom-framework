/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_gtk4/test_project_window.c
 *
 * PURPOSE:
 *   Verify project window identity and refusal of reentrant construction mutations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/kanban_project.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#ifdef _WIN32
#define ROOT "C:/Projects/editor"
#else
#define ROOT "/projects/editor"
#endif
static GtkWidget *Find(GtkWidget *root, const char *name) {
  if (!strcmp(gtk_widget_get_name(root), name))
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = Find(child, name);
    if (found)
      return found;
  }
  return NULL;
}
typedef struct Observer {
  const char *mode;
  unsigned fired;
} Observer;
static void TitleChanged(GObject *object, GParamSpec *spec, gpointer data) {
  (void)spec;
  Observer *o = data;
  const char *title = gtk_window_get_title(GTK_WINDOW(object));
  if (o->fired || title == NULL || !g_str_has_prefix(title, "Project board"))
    return;
  ++o->fired;
  if (!strcmp(o->mode, "retire"))
    gtk_window_destroy(GTK_WINDOW(object));
  else if (!strcmp(o->mode, "edit")) {
    GtkWidget *field = Find(GTK_WIDGET(object), "kanban-document-filename");
    if (field)
      gtk_editable_set_text(GTK_EDITABLE(field), "changed.umikb");
  } else if (!strcmp(o->mode, "start")) {
    GtkWidget *controls = Find(GTK_WIDGET(object), "kanban-document-controls");
    if (controls)
      for (GtkWidget *child = gtk_widget_get_first_child(controls); child;
           child = gtk_widget_get_next_sibling(child))
        if (GTK_IS_BOX(child))
          for (GtkWidget *button = gtk_widget_get_first_child(child); button;
               button = gtk_widget_get_next_sibling(button))
            if (GTK_IS_BUTTON(button) &&
                !g_strcmp0(gtk_button_get_label(GTK_BUTTON(button)), "Create board"))
              g_signal_emit_by_name(button, "clicked");
  }
}
static void WindowAdded(GtkApplication *app, GtkWindow *window, gpointer data) {
  (void)app;
  g_signal_connect(window, "notify::title", G_CALLBACK(TitleChanged), data);
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1];
  const char *cases[] = {"project",  "unicode", "copy", "generic", "closed",  "arguments",
                         "relative", "retire",  "edit", "start",   "metadata"};
  int known = 0;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int failed = 0;
  int held = 0;
  GtkApplication *app =
      gtk_application_new("org.umicom.ProjectBoardTest", G_APPLICATION_NON_UNIQUE);
  GtkWindow *window = NULL;
  CHECK(g_application_register(G_APPLICATION(app), NULL, NULL));
  UmiKanbanProjectLocation value, sentinel;
  memset(&value, 0x37, sizeof value);
  sentinel = value;
  if (!strcmp(name, "arguments")) {
    CHECK(UmiKanbanGtk4CreateProjectWindow(NULL, ROOT, &window) == UMI_STATUS_INVALID_ARGUMENT &&
          window == NULL);
    CHECK(UmiKanbanGtk4CreateProjectWindow(app, ROOT, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiKanbanGtk4ProjectLocation(NULL, &value) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!memcmp(&value, &sentinel, sizeof value));
    goto cleanup;
  }
  if (!strcmp(name, "generic")) {
    window = UmiKanbanGtk4CreateWindow(app);
    CHECK(window != NULL);
    g_object_ref(window);
    held = 1;
    CHECK(UmiKanbanGtk4ProjectLocation(window, &value) == UMI_STATUS_NOT_FOUND);
    CHECK(!memcmp(&value, &sentinel, sizeof value));
    goto cleanup;
  }
  Observer observer = {name, 0U};
  gulong signal = g_signal_connect(app, "window-added", G_CALLBACK(WindowAdded), &observer);
  const char *root = !strcmp(name, "unicode")    ? ROOT "-\xc3\xa9"
                     : !strcmp(name, "relative") ? "relative"
                                                 : ROOT;
  UmiStatus status = UmiKanbanGtk4CreateProjectWindow(app, root, &window);
  g_signal_handler_disconnect(app, signal);
  if (!strcmp(name, "relative") || !strcmp(name, "retire") || !strcmp(name, "edit")) {
    CHECK(status == (!strcmp(name, "retire") ? UMI_STATUS_CANCELLED
                     : !strcmp(name, "edit") ? UMI_STATUS_INVALID_STATE
                                             : UMI_STATUS_INVALID_ARGUMENT));
    CHECK(window == NULL);
    goto cleanup;
  }
  CHECK(status == UMI_STATUS_OK && window != NULL);
  g_object_ref(window);
  held = 1;
  CHECK(!UmiKanbanGtk4WindowBusy(window));
  CHECK(UmiKanbanGtk4ProjectLocation(window, &value) == UMI_STATUS_OK &&
        !strcmp(value.project_root, root));
  GtkWidget *directory = Find(GTK_WIDGET(window), "kanban-document-directory"),
            *filename = Find(GTK_WIDGET(window), "kanban-document-filename");
  CHECK(GTK_IS_EDITABLE(directory) && GTK_IS_EDITABLE(filename));
  CHECK(gtk_editable_get_text(GTK_EDITABLE(directory))[0] == '\0');
  CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(filename)), value.filename));
  CHECK(!gtk_widget_get_visible(GTK_WIDGET(window)));
  if (!strcmp(name, "copy")) {
    value.project_root[0] = 'x';
    CHECK(UmiKanbanGtk4ProjectLocation(window, &value) == UMI_STATUS_OK &&
          !strcmp(value.project_root, root));
  }
  if (!strcmp(name, "metadata")) {
    gtk_editable_set_text(GTK_EDITABLE(filename), "different.umikb");
    CHECK(UmiKanbanGtk4ProjectLocation(window, &value) == UMI_STATUS_OK &&
          strcmp(value.filename, "different.umikb"));
  }
  if (!strcmp(name, "start"))
    CHECK(observer.fired == 1U && !UmiKanbanGtk4WindowBusy(window));
  if (!strcmp(name, "closed")) {
    gtk_window_destroy(window);
    value = sentinel;
    CHECK(UmiKanbanGtk4ProjectLocation(window, &value) == UMI_STATUS_CANCELLED);
    CHECK(!memcmp(&value, &sentinel, sizeof value));
  }
cleanup:
  if (window != NULL) {
    gtk_window_destroy(window);
    /* Only release a reference acquired by this fixture. The application owns
     * any unexpected window returned on a failed factory path. */
    if (held)
      g_object_unref(window);
  }
  g_object_unref(app);
  return failed;
}
