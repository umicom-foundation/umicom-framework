/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/local_file_drop_gtk4.c
 *
 * PURPOSE:
 *   Capture local drop paths before invoking an application-owned workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/local_file_drop.h"
#include "umicom/ui/gtk4/window_lifecycle.h"

typedef struct LocalFileDrop {
  size_t maximumFiles;
  UmiLocalFileDropGtk4Receiver receiver;
  gpointer context;
  GDestroyNotify destroy;
  gboolean receiving;
} LocalFileDrop;

static void ReleaseLocalFileDrop(gpointer data) {
  LocalFileDrop *state = data;
  if (state->destroy != NULL)
    state->destroy(state->context);
  g_free(state);
}

static gboolean OwnerCanReceive(GtkDropTarget *target, GtkWidget *widget, GtkRoot *root) {
  return widget != NULL && root != NULL && GTK_IS_WINDOW(root) &&
         gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(target)) == widget &&
         gtk_widget_get_root(widget) == root && gtk_widget_get_mapped(widget) &&
         gtk_widget_is_sensitive(widget) && UmiGtk4WindowIsOpen(GTK_WINDOW(root));
}

/* Retain the borrowed-list implementation for review. The active receiver
 * owns a boxed list during conversion, so a provider callback cannot retire
 * the transfer value while its remaining GFile entries are being visited. */
#if 0
static gboolean ReceiveLocalFiles(GtkDropTarget *target, const GValue *value,
    double x, double y, gpointer data)
{
    (void)x;
    (void)y;
    LocalFileDrop *state = data;
    if (state->receiving || value == NULL || !G_VALUE_HOLDS(value, GDK_TYPE_FILE_LIST))
        return FALSE;
    GdkFileList *list = g_value_get_boxed(value);
    if (list == NULL) return FALSE;
    GtkWidget *widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(target));
    GtkRoot *root = widget != NULL ? gtk_widget_get_root(widget) : NULL;
    if (!OwnerCanReceive(target, widget, root)) return FALSE;

    /* Retain the controller before touching provider objects: a GFile may be
     * implemented by a plugin, and the receiver may remove its own controller.
     * Neither operation may release callback state before this stack unwinds. */
    g_object_ref(target);
    g_object_ref(widget);
    g_object_ref(root);
    state->receiving = TRUE;
    char *paths[UMI_LOCAL_FILE_DROP_MAXIMUM] = {0};
    size_t count = 0U;
    gboolean accepted = FALSE;
    for (GSList *item = gdk_file_list_get_files(list); item != NULL; item = item->next) {
        if (count >= state->maximumFiles || !G_IS_FILE(item->data)) goto done;
        paths[count] = g_file_get_path(G_FILE(item->data));
        if (paths[count] == NULL) goto done;
        size_t length = 0U;
        while (length < UMI_LOCAL_FILE_DROP_PATH_CAPACITY && paths[count][length] != '\0')
            ++length;
        if (length == UMI_LOCAL_FILE_DROP_PATH_CAPACITY ||
            !g_path_is_absolute(paths[count]) || !g_utf8_validate(paths[count], -1, NULL))
            goto done;
        ++count;
    }
    /* Recheck after conversion before calling application code. All paths are
     * owned copies, independent of the GValue and the original GFile list. */
    if (count != 0U && OwnerCanReceive(target, widget, root))
        accepted = state->receiver(widget, (const char *const *)paths, count,
            state->context) == UMI_STATUS_OK;
done:
    for (size_t i = 0U; i < UMI_LOCAL_FILE_DROP_MAXIMUM; ++i) g_free(paths[i]);
    state->receiving = FALSE;
    g_object_unref(root);
    g_object_unref(widget);
    g_object_unref(target);
    return accepted;
}
#endif
static gboolean ReceiveLocalFiles(GtkDropTarget *target, const GValue *value, double x, double y,
                                  gpointer data) {
  (void)x;
  (void)y;
  LocalFileDrop *state = data;
  if (state->receiving || value == NULL || !G_VALUE_HOLDS(value, GDK_TYPE_FILE_LIST))
    return FALSE;
  GdkFileList *list = g_value_get_boxed(value);
  if (list == NULL)
    return FALSE;
  GtkWidget *widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(target));
  GtkRoot *root = widget != NULL ? gtk_widget_get_root(widget) : NULL;
  if (!OwnerCanReceive(target, widget, root))
    return FALSE;

  /* Retain the controller before touching provider objects: a GFile may be
   * implemented by a plugin, and the receiver may remove its own controller.
   * Neither operation may release callback state before this stack unwinds. */
  g_object_ref(target);
  g_object_ref(widget);
  g_object_ref(root);
  state->receiving = TRUE;
  GdkFileList *ownedList = g_boxed_copy(GDK_TYPE_FILE_LIST, list);
  char *paths[UMI_LOCAL_FILE_DROP_MAXIMUM] = {0};
  size_t count = 0U;
  gboolean accepted = FALSE;
  for (GSList *item = gdk_file_list_get_files(ownedList); item != NULL; item = item->next) {
    if (count >= state->maximumFiles || !G_IS_FILE(item->data))
      goto done;
    paths[count] = g_file_get_path(G_FILE(item->data));
    if (paths[count] == NULL)
      goto done;
    size_t length = 0U;
    while (length < UMI_LOCAL_FILE_DROP_PATH_CAPACITY && paths[count][length] != '\0')
      ++length;
    if (length == UMI_LOCAL_FILE_DROP_PATH_CAPACITY || !g_path_is_absolute(paths[count]) ||
        !g_utf8_validate(paths[count], -1, NULL))
      goto done;
    ++count;
  }
  /* Recheck after conversion before calling application code. All paths are
   * owned copies, independent of the GValue and the original GFile list. */
  if (count != 0U && OwnerCanReceive(target, widget, root))
    accepted =
        state->receiver(widget, (const char *const *)paths, count, state->context) == UMI_STATUS_OK;
done:
  for (size_t i = 0U; i < UMI_LOCAL_FILE_DROP_MAXIMUM; ++i)
    g_free(paths[i]);
  g_boxed_free(GDK_TYPE_FILE_LIST, ownedList);
  state->receiving = FALSE;
  g_object_unref(root);
  g_object_unref(widget);
  g_object_unref(target);
  return accepted;
}

GtkDropTarget *UmiLocalFileDropGtk4Create(size_t maximumFiles,
                                          UmiLocalFileDropGtk4Receiver receiver, gpointer context,
                                          GDestroyNotify destroy) {
  if (maximumFiles == 0U || maximumFiles > UMI_LOCAL_FILE_DROP_MAXIMUM || receiver == NULL)
    return NULL;
  LocalFileDrop *state = g_try_new0(LocalFileDrop, 1);
  if (state == NULL)
    return NULL;
  state->maximumFiles = maximumFiles;
  state->receiver = receiver;
  state->context = context;
  state->destroy = destroy;
  GtkDropTarget *target = gtk_drop_target_new(GDK_TYPE_FILE_LIST, GDK_ACTION_COPY);
  g_object_set_data_full(G_OBJECT(target), "umicom-local-file-drop", state, ReleaseLocalFileDrop);
  gtk_event_controller_set_name(GTK_EVENT_CONTROLLER(target), "umicom.local-files");
  g_signal_connect(target, "drop", G_CALLBACK(ReceiveLocalFiles), state);
  return target;
}
