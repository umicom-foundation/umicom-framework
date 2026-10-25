/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/editor_indentation.inc
 *
 * PURPOSE:
 *   Project copied indentation options into retained GTK text views without retaining an
 * application owner across notifications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/indentation.h"
#include "umicom/document/indentation_override.h"
#include "umicom/ui/gtk4/editor_indentation.h"
static void EditorApplyIndentation(GtkWidget *view, const UmiEditorIndentation *policy) {
  g_object_freeze_notify(G_OBJECT(view));
#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
  if (GTK_SOURCE_IS_VIEW(view)) {
    GtkSourceView *source = GTK_SOURCE_VIEW(view);
    if (gtk_source_view_get_tab_width(source) != policy->tab_width)
      gtk_source_view_set_tab_width(source, policy->tab_width);
    if (gtk_source_view_get_indent_width(source) != (gint)policy->tab_width)
      gtk_source_view_set_indent_width(source, (gint)policy->tab_width);
    if (gtk_source_view_get_insert_spaces_instead_of_tabs(source) !=
        (gboolean)policy->insert_spaces)
      gtk_source_view_set_insert_spaces_instead_of_tabs(source, policy->insert_spaces != 0);
  } else
#endif
  {
    /* Plain GtkTextView needs measured tab stops. Input still uses the
     * same Framework Tab command, rather than toolkit-specific history. */
    char spaces[UMI_EDITOR_INDENTATION_MAXIMUM_WIDTH + 1U];
    memset(spaces, ' ', policy->tab_width);
    spaces[policy->tab_width] = '\0';
    PangoLayout *layout = gtk_widget_create_pango_layout(view, spaces);
    int width = 0;
    pango_layout_get_pixel_size(layout, &width, NULL);
    g_object_unref(layout);
    if (width < 1)
      width = 1;
    PangoTabArray *previous = gtk_text_view_get_tabs(GTK_TEXT_VIEW(view));
    PangoTabAlign alignment = PANGO_TAB_LEFT;
    int location = 0;
    if (previous != NULL && pango_tab_array_get_size(previous) == 1)
      pango_tab_array_get_tab(previous, 0, &alignment, &location);
    if (previous == NULL || pango_tab_array_get_size(previous) != 1 ||
        !pango_tab_array_get_positions_in_pixels(previous) || alignment != PANGO_TAB_LEFT ||
        location != width) {
      PangoTabArray *tabs = pango_tab_array_new(1, TRUE);
      pango_tab_array_set_tab(tabs, 0, PANGO_TAB_LEFT, width);
      gtk_text_view_set_tabs(GTK_TEXT_VIEW(view), tabs);
      pango_tab_array_free(tabs);
    }
    if (previous != NULL)
      pango_tab_array_free(previous);
  }
  /* A notification can retire the host. No application pointer is used here. */
  g_object_thaw_notify(G_OBJECT(view));
}
/* The global-only projector is retained for engineering comparison. Each
 * native view now receives a checked policy from its own working copy. */
#if 0
UmiStatus UmiGtk4AdapterRefreshIndentation(UmiGtk4Adapter *adapter) {
  if (adapter == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL)
    return UMI_STATUS_OK;
  UmiEditorIndentation policy;
  UmiStatus status = UmiDocumentCoordinatorGetIndentation(adapter->edit_coordinator, &policy);
  if (status != UMI_STATUS_OK)
    return status;
  GObject *lifetime = g_object_ref(adapter->edit_lifetime);
  GPtrArray *views = g_ptr_array_new_with_free_func(g_object_unref);
  GtkWidget *notebooks[] = {adapter->document_notebook, adapter->secondary_document_notebook};
  for (size_t group = 0U; group < sizeof notebooks / sizeof notebooks[0]; ++group) {
    if (!GTK_IS_NOTEBOOK(notebooks[group]))
      continue;
    for (int index = 0; index < gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebooks[group])); ++index) {
      GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebooks[group]), index);
      GtkWidget *view = g_object_get_data(G_OBJECT(page), "umicom-editor-view");
      if (GTK_IS_TEXT_VIEW(view))
        g_ptr_array_add(views, g_object_ref(view));
    }
  }
  /* Native refs and the copied policy survive property observers. Never
   * traverse the adapter or notebook again once notifications can run. */
  for (guint index = 0U; index < views->len; ++index) {
    if (g_object_get_data(lifetime, "adapter") == NULL) {
      status = UMI_STATUS_CANCELLED;
      break;
    }
    EditorApplyIndentation(g_ptr_array_index(views, index), &policy);
    UmiGtk4Adapter *current = g_object_get_data(lifetime, "adapter");
    if (current == NULL) {
      status = UMI_STATUS_CANCELLED;
      break;
    }
    UmiEditorIndentation observed;
    status = UmiDocumentCoordinatorGetIndentation(current->edit_coordinator, &observed);
    if (status != UMI_STATUS_OK)
      break;
    /* An observer may change preferences synchronously. Stop before the
     * old snapshot can overwrite another view with stale presentation. */
    if (observed.tab_width != policy.tab_width || observed.insert_spaces != policy.insert_spaces) {
      status = UMI_STATUS_INVALID_STATE;
      break;
    }
  }
  if (g_object_get_data(lifetime, "adapter") == NULL)
    status = UMI_STATUS_CANCELLED;
  g_ptr_array_unref(views);
  g_object_unref(lifetime);
  return status;
}

#endif

typedef struct EditorIndentationProjection {
    GtkWidget *page;
    GtkWidget *view;
    char view_id[UMI_UI_ID_CAPACITY];
    UmiDocumentIndentationReview review;
    UmiEditorIndentation fallback;
    const UmiDocumentCoordinator *owner;
    int managed;
} EditorIndentationProjection;
static void EditorIndentationProjectionFree(gpointer data) {
    EditorIndentationProjection *item = data;
    g_object_unref(item->page);
    g_object_unref(item->view);
    g_free(item);
}
static UmiStatus EditorIndentationDocument(UmiDocumentCoordinator *owner,
    const char *viewId, UmiDocumentIndentationReview *review) {
    for (size_t i = 0U; i < umi_document_coordinator_count(owner); ++i) {
        UmiDocumentWorkingCopySnapshot document;
        UmiStatus status = umi_document_coordinator_at(owner, i, &document);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(document.view_id, viewId) == 0) {
            status = UmiDocumentCoordinatorInspectIndentation(owner, document.document_id, review);
            /* A managed view missing its projection is not an unmanaged fallback. */
            return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_INVALID_STATE : status;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
static UmiStatus EditorIndentationProjectionCheck(UmiGtk4Adapter *adapter,
    const EditorIndentationProjection *item) {
    if (adapter->edit_coordinator != item->owner) return UMI_STATUS_INVALID_STATE;
    const char *viewId = g_object_get_data(G_OBJECT(item->page), "umicom-view-id");
    if (g_strcmp0(viewId, item->view_id) != 0 ||
        g_object_get_data(G_OBJECT(item->page), "umicom-editor-view") != item->view)
        return UMI_STATUS_INVALID_STATE;
    UmiDocumentIndentationReview observed;
    UmiStatus status = EditorIndentationDocument(adapter->edit_coordinator, item->view_id, &observed);
    if (item->managed) {
        if (status != UMI_STATUS_OK) return status;
        if (observed.document_id != item->review.document_id) return UMI_STATUS_INVALID_STATE;
        return UmiDocumentCoordinatorCheckIndentation(adapter->edit_coordinator, &item->review);
    }
    if (status != UMI_STATUS_NOT_FOUND)
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    UmiEditorIndentation fallback;
    status = UmiDocumentCoordinatorGetIndentation(adapter->edit_coordinator, &fallback);
    if (status == UMI_STATUS_OK &&
        (fallback.tab_width != item->fallback.tab_width ||
         fallback.insert_spaces != item->fallback.insert_spaces)) return UMI_STATUS_INVALID_STATE;
    return status;
}
UmiStatus UmiGtk4AdapterRefreshIndentation(UmiGtk4Adapter *adapter) {
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL)
        return UMI_STATUS_OK;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GPtrArray *items = g_ptr_array_new_with_free_func(EditorIndentationProjectionFree);
    UmiStatus status = UMI_STATUS_OK;
    GtkWidget *notebooks[] = {adapter->document_notebook, adapter->secondary_document_notebook};
    for (size_t group = 0U; status == UMI_STATUS_OK && group < sizeof notebooks / sizeof notebooks[0]; ++group) {
        if (!GTK_IS_NOTEBOOK(notebooks[group])) continue;
        for (int i = 0; status == UMI_STATUS_OK && i < gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebooks[group])); ++i) {
            GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebooks[group]), i);
            GtkWidget *view = g_object_get_data(G_OBJECT(page), "umicom-editor-view");
            if (!GTK_IS_TEXT_VIEW(view)) continue;
            const char *viewId = g_object_get_data(G_OBJECT(page), "umicom-view-id");
            if (viewId == NULL || strlen(viewId) >= UMI_UI_ID_CAPACITY) {
                status = UMI_STATUS_INVALID_STATE;
                break;
            }
            EditorIndentationProjection *item = g_new0(EditorIndentationProjection, 1);
            item->page = g_object_ref(page);
            item->view = g_object_ref(view);
            item->owner = adapter->edit_coordinator;
            g_strlcpy(item->view_id, viewId, sizeof item->view_id);
            g_ptr_array_add(items, item);
            status = EditorIndentationDocument(adapter->edit_coordinator, viewId, &item->review);
            item->managed = status == UMI_STATUS_OK;
            if (status == UMI_STATUS_NOT_FOUND)
                status = UmiDocumentCoordinatorGetIndentation(adapter->edit_coordinator, &item->fallback);
        }
    }
    /* Capture every policy and retain native objects before emitting property
     * notifications. Recheck every captured document after each projection:
     * observers can change a different tab, close it, or retire/rebind the host. */
    for (guint i = 0U; status == UMI_STATUS_OK && i < items->len; ++i) {
        UmiGtk4Adapter *current = g_object_get_data(lifetime, "adapter");
        if (current == NULL) { status = UMI_STATUS_CANCELLED; break; }
        EditorIndentationProjection *item = g_ptr_array_index(items, i);
        status = EditorIndentationProjectionCheck(current, item);
        if (status != UMI_STATUS_OK) break;
        EditorApplyIndentation(item->view, item->managed ? &item->review.effective : &item->fallback);
        current = g_object_get_data(lifetime, "adapter");
        if (current == NULL) { status = UMI_STATUS_CANCELLED; break; }
        for (guint checked = 0U; status == UMI_STATUS_OK && checked < items->len; ++checked)
            status = EditorIndentationProjectionCheck(current, g_ptr_array_index(items, checked));
    }
    if (g_object_get_data(lifetime, "adapter") == NULL) status = UMI_STATUS_CANCELLED;
    g_ptr_array_unref(items);
    g_object_unref(lifetime);
    return status;
}
