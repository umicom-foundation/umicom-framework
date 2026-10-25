/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_binding.inc
 *
 * PURPOSE:
 *   Bind private location controls to the existing board worker and window lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/kanban_binding.h"
#include "umicom/platform/resource_location.h"
UmiStatus UmiKanbanGtk4EnableProjectStorage(GtkWindow *window, const char *directory)
{
    if (!GTK_IS_WINDOW(window) || directory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    WindowState *state = g_object_get_data(G_OBJECT(window), "umicom-kanban-window");
    if (state == NULL) return UMI_STATUS_NOT_FOUND;
    if (state->closed || gtk_window_get_application(window) == NULL) return UMI_STATUS_CANCELLED;
    if (state->busy || state->document != NULL) return UMI_STATUS_BUSY;
    if (g_object_get_data(G_OBJECT(window), "umicom-kanban-settings") != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    UmiKanbanProjectLocation project;
    UmiStatus status = UmiKanbanGtk4ProjectLocation(window, &project);
    char path[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = UmiKanbanBindingStoragePath(directory, project.project_root, path, sizeof path);
    if (status != UMI_STATUS_OK) return status;
    if (!g_utf8_validate(directory, -1, NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep admission and a window reference through notifying GTK calls.
     * Controls reuse the existing serialized worker and close ownership. */
    /* Callers may supply editable text. Keep our own copy before GTK
     * notifications can replace that borrowed storage. */
    char *owned_directory = g_strdup(directory);
    g_object_ref(window);
    state->busy = true;
    g_object_set_data_full(G_OBJECT(window), "umicom-kanban-settings", g_strdup(directory), g_free);
    const char *labels[] = {"Remember entered location", "Recall saved location"};
    const char *names[] = {"kanban-project-remember", "kanban-project-recall"};
    const JobKind kinds[] = {JOB_BINDING_SAVE, JOB_BINDING_LOAD};
    for (size_t i = 0U; i < 2U; ++i) {
        GtkWidget *button = gtk_button_new_with_label(labels[i]);
        g_object_ref_sink(button);
        gtk_widget_set_name(button, names[i]);
        g_object_set_data(G_OBJECT(button), "kanban-job", GINT_TO_POINTER(kinds[i]));
        g_signal_connect_object(button, "clicked", G_CALLBACK(Click), window, 0);
        if (!state->closed) gtk_box_append(GTK_BOX(state->controls), button);
        g_object_unref(button);
        if (state->closed) { status = UMI_STATUS_CANCELLED; break; }
    }
    if (status == UMI_STATUS_OK) {
        /* The copied path replaces borrowed input after native notifications.
         * Retain the earlier expression for lifetime review. */
#if 0
        char *message = g_strdup_printf("Remember or recall a location while the board is closed. "
            "Recall fills these fields; choose Open separately. Private location settings: %s", directory);
#endif
        char *message = g_strdup_printf("Remember or recall a location while the board is closed. "
            "Recall fills these fields; choose Open separately. Private location settings: %s", owned_directory);
        Say(state, message);
        g_free(message);
        if (state->closed) status = UMI_STATUS_CANCELLED;
    }
    state->busy = false;
    g_free(owned_directory);
    g_object_unref(window);
    return status;
}
UmiStatus UmiKanbanGtk4EnableProjectStorageForApplication(GtkWindow *window,
    const char *application, const char *base)
{
    if (!GTK_IS_WINDOW(window) || application == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiApplicationPathsConfig config = UmiApplicationPathsConfigDefault(application);
    config.baseOverride = base;
    UmiApplicationPaths *paths = g_new0(UmiApplicationPaths, 1);
    UmiStatus status = UmiApplicationPathsResolve(&config, paths);
    if (status == UMI_STATUS_OK) status = UmiKanbanGtk4EnableProjectStorage(window, paths->config);
    g_free(paths);
    if (status != UMI_STATUS_OK) {
        WindowState *state = g_object_get_data(G_OBJECT(window), "umicom-kanban-window");
        if (state != NULL && !state->closed && !state->busy) {
            g_object_ref(window);
            state->busy = true;
            Say(state, "Remembered locations are unavailable. Choose a folder and use Open/Create manually.");
            state->busy = false;
            g_object_unref(window);
        }
    }
    return status;
}
