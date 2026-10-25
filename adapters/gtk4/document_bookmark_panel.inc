/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_bookmark_panel.inc
 *
 * PURPOSE:
 *   Render and activate copied bookmark rows without retaining product runtime state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/bookmark_panel.h"
#include "umicom/ui/gtk4/list_publication.h"

typedef struct BookmarkPanelState {
  GtkWidget *root, *list, *summary;
  GObject *lifetime, *published;
  UmiGtk4BookmarkPanelResult result;
  void *context;
  GDestroyNotify destroy;
  int busy;
} BookmarkPanelState;
typedef struct BookmarkPanelCapture {
  UmiDocumentCoordinator *documents; /* Live binding identity, never ownership. */
  UmiDocumentBookmarksSnapshot summary;
  UmiDocumentBookmark rows[UMI_DOCUMENT_BOOKMARK_CAPACITY];
} BookmarkPanelCapture;
typedef struct BookmarkPanelBinding {
  GWeakRef root;
} BookmarkPanelBinding;
typedef struct BookmarkPanelRow {
  GObject *capture;
  size_t index;
  int remove;
} BookmarkPanelRow;
static UmiGtk4Adapter *BookmarkPanelOwner(BookmarkPanelState *state) {
  UmiGtk4Adapter *adapter = g_object_get_data(state->lifetime, "adapter");
  return adapter != NULL && adapter->edit_lifetime == state->lifetime &&
                 adapter->edit_coordinator != NULL && UmiGtk4WindowIsOpen(adapter->window)
             ? adapter
             : NULL;
}
static void BookmarkPanelFree(gpointer data) {
  BookmarkPanelState *state = data;
  g_clear_object(&state->published);
  g_clear_object(&state->lifetime);
  g_clear_object(&state->list);
  g_clear_object(&state->summary);
  if (state->destroy != NULL)
    state->destroy(state->context);
  g_free(state);
}
static void BookmarkPanelBindingFree(gpointer data, GClosure *closure) {
  (void)closure;
  BookmarkPanelBinding *binding = data;
  g_weak_ref_clear(&binding->root);
  g_free(binding);
}
static void BookmarkPanelRowFree(gpointer data) {
  BookmarkPanelRow *row = data;
  g_object_unref(row->capture);
  g_free(row);
}
static void BookmarkPanelBind(GtkWidget *button, GCallback callback, GtkWidget *root) {
  BookmarkPanelBinding *binding = g_new0(BookmarkPanelBinding, 1);
  g_weak_ref_init(&binding->root, root);
  g_signal_connect_data(button, "clicked", callback, binding, BookmarkPanelBindingFree, 0);
}
typedef struct BookmarkPanelPublication {
  BookmarkPanelState *state;
  GObject *token;
} BookmarkPanelPublication;
static UmiStatus BookmarkPanelCurrent(GtkListBox *list, void *data) {
  BookmarkPanelPublication *publication = data;
  BookmarkPanelState *state = publication->state;
  BookmarkPanelCapture *capture = g_object_get_data(publication->token, "capture");
  UmiGtk4Adapter *adapter = BookmarkPanelOwner(state);
  if (adapter == NULL || GTK_WIDGET(list) != state->list)
    return UMI_STATUS_CANCELLED;
  if (adapter->edit_coordinator != capture->documents)
    return UMI_STATUS_INVALID_STATE;
  UmiDocumentBookmarksSnapshot now;
  UmiStatus status = UmiDocumentCoordinatorBookmarks(adapter->edit_coordinator, &now);
  if (status != UMI_STATUS_OK)
    return status;
  if (now.busy || adapter->edit_cancel != NULL || UmiGtk4AdapterDocumentSaveAllBusy(adapter) ||
      UmiGtk4AdapterDocumentCloseBusy(adapter) || UmiGtk4AdapterReplacementReviewBusy(adapter))
    return UMI_STATUS_BUSY;
  return now.revision == capture->summary.revision && now.count == capture->summary.count &&
                 now.open_count == capture->summary.open_count
             ? UMI_STATUS_OK
             : UMI_STATUS_INVALID_STATE;
}
static void BookmarkPanelClicked(GtkButton *button, gpointer data) {
  BookmarkPanelBinding *binding = data;
  GtkWidget *root = g_weak_ref_get(&binding->root);
  if (root == NULL)
    return;
  BookmarkPanelState *state = g_object_get_data(G_OBJECT(root), "umicom-bookmark-panel");
  BookmarkPanelRow *row = g_object_get_data(G_OBJECT(button), "umicom-bookmark-row");
  UmiGtk4Adapter *adapter = state != NULL ? BookmarkPanelOwner(state) : NULL;
  GtkWidget *native_row = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_LIST_BOX_ROW);
  if (adapter != NULL && row != NULL && !state->busy && state->published == row->capture &&
      native_row != NULL && gtk_widget_get_parent(native_row) == state->list &&
      (gpointer)gtk_widget_get_root(GTK_WIDGET(button)) == (gpointer)adapter->window) {
    GObject *token = g_object_ref(row->capture);
    BookmarkPanelPublication publication = {state, token};
    UmiStatus status = BookmarkPanelCurrent(GTK_LIST_BOX(state->list), &publication);
    BookmarkPanelCapture *capture = g_object_get_data(token, "capture");
    size_t index = row->index;
    int remove = row->remove;
    state->busy = 1;
    if (status == UMI_STATUS_OK) {
      /* Copy all row identity before the document completion callback can
       * replace this list, reveal another panel or retire its host. */
      status = remove ? UmiDocumentCoordinatorRemoveBookmark(adapter->edit_coordinator, index,
                                                             capture->summary.revision)
                      : UmiDocumentCoordinatorVisitBookmark(adapter->edit_coordinator, index,
                                                            capture->summary.revision);
      status = EditFinished(adapter, status);
    }
    adapter = BookmarkPanelOwner(state);
    if (adapter != NULL && state->result != NULL)
      state->result(state->context, status, !remove);
    state->busy = 0;
    if (BookmarkPanelOwner(state) != NULL) {
      (void)UmiGtk4BookmarkPanelRefresh(root);
      if (status != UMI_STATUS_OK && BookmarkPanelOwner(state) != NULL) {
        state->busy = 1;
        gtk_label_set_text(GTK_LABEL(state->summary),
                           status == UMI_STATUS_NOT_FOUND
                               ? "The bookmarked document or line is no longer available. Remove "
                                 "the marker or refresh."
                               : "The bookmark action was refused. Refresh the list and finish any "
                                 "active document operation.");
        state->busy = 0;
      }
    }
    g_object_unref(token);
  }
  g_object_unref(root);
}
static void BookmarkPanelRefreshClicked(GtkButton *button, gpointer data) {
  BookmarkPanelBinding *binding = data;
  GtkWidget *root = g_weak_ref_get(&binding->root);
  if (root == NULL)
    return;
  BookmarkPanelState *state = g_object_get_data(G_OBJECT(root), "umicom-bookmark-panel");
  UmiGtk4Adapter *adapter = state != NULL ? BookmarkPanelOwner(state) : NULL;
  if (adapter != NULL &&
      (gpointer)gtk_widget_get_root(GTK_WIDGET(button)) == (gpointer)adapter->window)
    (void)UmiGtk4BookmarkPanelRefresh(root);
  g_object_unref(root);
}
UmiStatus UmiGtk4BookmarkPanelRefresh(GtkWidget *root) {
  if (root == NULL || !GTK_IS_WIDGET(root))
    return UMI_STATUS_INVALID_ARGUMENT;
  BookmarkPanelState *state = g_object_get_data(G_OBJECT(root), "umicom-bookmark-panel");
  if (state == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (state->busy)
    return UMI_STATUS_BUSY;
  UmiGtk4Adapter *adapter = BookmarkPanelOwner(state);
  if (adapter == NULL)
    return UMI_STATUS_CANCELLED;
  g_object_ref(root);
  state->busy = 1;
  GObject *token = g_object_new(G_TYPE_OBJECT, NULL);
  BookmarkPanelCapture *capture = g_new0(BookmarkPanelCapture, 1);
  g_object_set_data_full(token, "capture", capture, g_free);
  capture->documents = adapter->edit_coordinator;
  UmiStatus status = UmiDocumentCoordinatorBookmarks(capture->documents, &capture->summary);
  GtkWidget *rows[UMI_DOCUMENT_BOOKMARK_CAPACITY] = {0};
  size_t count = 0U;
  BookmarkPanelPublication publication = {state, token};
  if (status == UMI_STATUS_OK)
    status = BookmarkPanelCurrent(GTK_LIST_BOX(state->list), &publication);
  for (size_t i = 0U; status == UMI_STATUS_OK && i < capture->summary.count; ++i)
    status = UmiDocumentCoordinatorBookmarkAt(capture->documents, i, &capture->rows[i]);
  if (status != UMI_STATUS_OK)
    goto finished;
  /* Completed snapshots are copied before the first signal-emitting widget.
   * Bookmarks remain session metadata, including unavailable closed markers. */
  char text[180];
  (void)snprintf(
      text, sizeof text,
      "%zu bookmarks; %zu in open documents. Locations are fixed until you replace a marker.",
      capture->summary.count, capture->summary.open_count);
  gtk_label_set_text(GTK_LABEL(state->summary), text);
  status = BookmarkPanelCurrent(GTK_LIST_BOX(state->list), &publication);
  if (status != UMI_STATUS_OK)
    goto finished;
  for (size_t i = 0U; i < capture->summary.count; ++i) {
    UmiDocumentBookmark *mark = &capture->rows[i];
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    g_object_ref_sink(box);
    rows[count++] = box;
    char *name = g_utf8_make_valid(mark->display_name, -1);
    char *label =
        g_strdup_printf("%s · %" G_GUINT64_FORMAT ":%" G_GUINT64_FORMAT "%s", name,
                        (guint64)mark->line, (guint64)mark->column, mark->open ? "" : " · closed");
    GtkWidget *jump = gtk_button_new_with_label(label),
              *remove = gtk_button_new_with_label("Remove marker");
    gtk_widget_set_hexpand(jump, TRUE);
    gtk_widget_set_sensitive(jump, mark->open);
    gtk_widget_set_tooltip_text(remove,
                                "Remove this session marker. Keep the document and its text.");
    for (int action = 0; action < 2; ++action) {
      GtkWidget *button = action ? remove : jump;
      BookmarkPanelRow *row = g_new0(BookmarkPanelRow, 1);
      row->capture = g_object_ref(token);
      row->index = i;
      row->remove = action;
      g_object_set_data_full(G_OBJECT(button), "umicom-bookmark-row", row, BookmarkPanelRowFree);
      g_object_set_data(G_OBJECT(button), "umicom-automation-id",
                        action ? "umicom-bookmark-remove" : "umicom-bookmark-visit");
      BookmarkPanelBind(button, G_CALLBACK(BookmarkPanelClicked), root);
    }
    gtk_box_append(GTK_BOX(box), jump);
    gtk_box_append(GTK_BOX(box), remove);
    g_free(name);
    g_free(label);
  }
  g_clear_object(&state->published);
  status = UmiGtk4ListBoxPublish(GTK_LIST_BOX(state->list), rows, count, BookmarkPanelCurrent,
                                 &publication);
  if (status == UMI_STATUS_OK)
    status = BookmarkPanelCurrent(GTK_LIST_BOX(state->list), &publication);
  if (status == UMI_STATUS_OK)
    state->published = g_object_ref(token);
finished:
  if (status != UMI_STATUS_OK)
    g_clear_object(&state->published);
  for (size_t i = 0U; i < count; ++i)
    g_object_unref(rows[i]);
  g_object_unref(token);
  state->busy = 0;
  g_object_unref(root);
  return status;
}
GtkWidget *UmiGtk4BookmarkPanelCreate(UmiGtk4Adapter *adapter, UmiGtk4BookmarkPanelResult result,
                                      void *context, GDestroyNotify destroy) {
  if (adapter == NULL || adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL ||
      !UmiGtk4WindowIsOpen(adapter->window))
    return NULL;
  /* Capture the invalidatable token before any GTK allocation can notify
   * observers. The product's runtime is never retained by this shared view. */
  GObject *lifetime = g_object_ref(adapter->edit_lifetime);
  GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  g_object_ref_sink(root);
  BookmarkPanelState *state = g_new0(BookmarkPanelState, 1);
  state->root = root;
  state->lifetime = lifetime;
  state->result = result;
  state->context = context;
  state->destroy = destroy;
  state->summary = gtk_label_new("Refresh to inspect session bookmarks.");
  g_object_ref_sink(state->summary);
  gtk_label_set_wrap(GTK_LABEL(state->summary), TRUE);
  gtk_label_set_xalign(GTK_LABEL(state->summary), 0.0F);
  state->list = gtk_list_box_new();
  g_object_ref_sink(state->list);
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(state->list), GTK_SELECTION_NONE);
  g_object_set_data(G_OBJECT(state->list), "umicom-automation-id", "umicom-bookmark-list");
  g_object_set_data(G_OBJECT(state->summary), "umicom-automation-id", "umicom-bookmark-summary");
  GtkWidget *refresh = gtk_button_new_with_label("Refresh bookmarks");
  g_object_set_data(G_OBJECT(refresh), "umicom-automation-id", "umicom-bookmark-refresh");
  g_object_set_data_full(G_OBJECT(root), "umicom-bookmark-panel", state, BookmarkPanelFree);
  BookmarkPanelBind(refresh, G_CALLBACK(BookmarkPanelRefreshClicked), root);
  gtk_box_append(GTK_BOX(root), state->summary);
  gtk_box_append(GTK_BOX(root), refresh);
  gtk_box_append(GTK_BOX(root), state->list);
  (void)UmiGtk4BookmarkPanelRefresh(root);
  return root;
}
