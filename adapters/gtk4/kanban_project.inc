/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_project.inc
 *
 * PURPOSE:
 *   Prepare project-specific board windows without starting file operations or borrowing the host
 * workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/kanban_project.h"
UmiStatus UmiKanbanGtk4CreateProjectWindow(GtkApplication *application, const char *projectRoot,
                                           GtkWindow **out) {
  if (!GTK_IS_APPLICATION(application) || out == NULL || *out != NULL || projectRoot == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiKanbanProjectLocation location;
  UmiStatus status = UmiKanbanProjectLocationCreate(projectRoot, &location);
  if (status != UMI_STATUS_OK)
    return status;
  /* GTK labels require UTF-8 even on platforms whose path layer admits
   * native byte names. Refuse rather than displaying a changed identity. */
  if (!g_utf8_validate(location.project_root, -1, NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  GtkWindow *window = UmiKanbanGtk4CreateWindow(application);
  if (window == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  g_object_ref(window);
  WindowState *state = g_object_get_data(G_OBJECT(window), "umicom-kanban-window");
  if (state == NULL || state->closed || gtk_window_get_application(window) != application) {
    status = UMI_STATUS_CANCELLED;
    goto refused;
  }
  /* Hold admission through notifying property changes. An observer cannot
   * start file work from this half-configured project window. */
  state->busy = true;
  UmiKanbanProjectLocation *owned = g_new(UmiKanbanProjectLocation, 1);
  *owned = location;
  g_object_set_data_full(G_OBJECT(window), "umicom-kanban-project", owned, g_free);
  gtk_editable_set_text(GTK_EDITABLE(state->filename), location.filename);
  if (state->closed || gtk_window_get_application(window) != application) {
    status = UMI_STATUS_CANCELLED;
    goto refused;
  }
  if (strcmp(gtk_editable_get_text(GTK_EDITABLE(state->filename)), location.filename) != 0) {
    status = UMI_STATUS_INVALID_STATE;
    goto refused;
  }
  char *title = g_strdup_printf("Project board — %s", location.project_root);
  gtk_window_set_title(window, title);
  g_free(title);
  if (state->closed || gtk_window_get_application(window) != application) {
    status = UMI_STATUS_CANCELLED;
    goto refused;
  }
  Say(state, "This board window keeps the displayed project identity. Choose a private folder, "
             "then Open its suggested filename or Create a new board. Nothing is saved inside the "
             "project automatically.");
  if (state->closed || gtk_window_get_application(window) != application) {
    status = UMI_STATUS_CANCELLED;
    goto refused;
  }
  if (strcmp(gtk_editable_get_text(GTK_EDITABLE(state->filename)), location.filename) != 0 ||
      gtk_editable_get_text(GTK_EDITABLE(state->directory))[0] != '\0') {
    status = UMI_STATUS_INVALID_STATE;
    goto refused;
  }
  state->busy = false;
  *out = window;
  g_object_unref(window);
  return UMI_STATUS_OK;
refused:
  if (state != NULL)
    state->busy = false;
  gtk_window_destroy(window);
  g_object_unref(window);
  return status;
}
UmiStatus UmiKanbanGtk4ProjectLocation(GtkWindow *window, UmiKanbanProjectLocation *out) {
  if (!GTK_IS_WINDOW(window) || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  WindowState *state = g_object_get_data(G_OBJECT(window), "umicom-kanban-window");
  if (state == NULL)
    return UMI_STATUS_NOT_FOUND;
  if (state->closed || gtk_window_get_application(window) == NULL)
    return UMI_STATUS_CANCELLED;
  const UmiKanbanProjectLocation *location =
      g_object_get_data(G_OBJECT(window), "umicom-kanban-project");
  if (location == NULL)
    return UMI_STATUS_NOT_FOUND;
  *out = *location;
  return UMI_STATUS_OK;
}
