/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/context_interaction_gtk4.c
 *
 * PURPOSE:
 *   Bind real GTK4 editor, Problems and Source Control interactions to the
 *   toolkit-neutral context-interaction sink exposed by the reference adapter.
 *
 * DESIGN:
 *   This module discovers native widgets after normal workbench rendering.
 *   GTK signal details stay entirely inside the adapter. Product composition
 *   receives only stable C callbacks containing document or selection data.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

#include <stdlib.h>
#include <string.h>

#define UMI_GTK4_CONTEXT_BOUND_KEY \
    "umicom-context-interaction-bound"
#define UMI_GTK4_CONTEXT_ADAPTER_KEY \
    "umicom-context-interaction-adapter"
#define UMI_GTK4_CONTEXT_VIEW_ID_KEY \
    "umicom-context-view-id"
#define UMI_GTK4_CONTEXT_SOURCE_KIND_KEY \
    "umicom-context-source-kind"
#define UMI_GTK4_CONTEXT_ROW_TEXT_KEY \
    "umicom-context-row-text"
#define UMI_GTK4_CONTEXT_VIEW_KIND_KEY \
    "umicom-context-view-kind"

static uint64_t now_ms(void)
{
    return (uint64_t)(g_get_monotonic_time() / 1000);
}

static UmiUiWorkbench *adapter_workbench(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL || adapter->shell == NULL) return NULL;
    return umi_ui_application_shell_workbench(adapter->shell);
}

static UmiStatus find_document(
    UmiGtk4Adapter *adapter,
    const char *view_id,
    UmiUiDocumentViewSnapshot *out_document)
{
    UmiUiDocumentViewModel *documents;
    UmiUiWorkbench *workbench;
    size_t index;

    if (adapter == NULL || view_id == NULL ||
        out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    workbench = adapter_workbench(adapter);
    if (workbench == NULL) return UMI_STATUS_INVALID_STATE;
    documents = umi_ui_workbench_documents(workbench);

    for (index = 0U;
         index < umi_ui_document_view_model_count(documents);
         ++index) {
        UmiUiDocumentViewSnapshot document;
        if (umi_ui_document_view_model_at(
                documents, index, &document) == UMI_STATUS_OK &&
            strcmp(document.view_id, view_id) == 0) {
            *out_document = document;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

static char *document_path(const UmiUiDocumentViewSnapshot *document)
{
    char *filename;
    if (document == NULL) return NULL;

    if (g_str_has_prefix(document->uri, "file://")) {
        filename = g_filename_from_uri(
            document->uri,
            NULL,
            NULL);
        if (filename != NULL) return filename;
    }
    return g_strdup(
        document->uri[0] != '\0'
            ? document->uri
            : document->document_id);
}

static GtkWidget *find_text_view(GtkWidget *root)
{
    GtkWidget *child;
    if (root == NULL) return NULL;
    if (GTK_IS_TEXT_VIEW(root)) return root;

    child = gtk_widget_get_first_child(root);
    while (child != NULL) {
        GtkWidget *found = find_text_view(child);
        if (found != NULL) return found;
        child = gtk_widget_get_next_sibling(child);
    }
    return NULL;
}

static UmiStatus emit_editor_location(
    UmiGtk4Adapter *adapter,
    GtkTextBuffer *buffer,
    const char *view_id)
{
    UmiUiDocumentViewSnapshot document;
    GtkTextIter caret;
    GtkTextIter selection_start;
    GtkTextIter selection_end;
    GtkTextMark *insert_mark;
    uint32_t line;
    uint32_t column;
    uint32_t selection_length = 0U;
    char *path;
    UmiStatus status;

    if (adapter == NULL || buffer == NULL || view_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!adapter->context_interactions_bound ||
        adapter->context_interaction_sink.editor_location == NULL) {
        return UMI_STATUS_OK;
    }

    status = find_document(adapter, view_id, &document);
    if (status != UMI_STATUS_OK) return status;

    insert_mark = gtk_text_buffer_get_insert(buffer);
    gtk_text_buffer_get_iter_at_mark(
        buffer,
        &caret,
        insert_mark);

    line = (uint32_t)gtk_text_iter_get_line(&caret) + 1U;
    column = (uint32_t)gtk_text_iter_get_line_offset(&caret) + 1U;

    if (gtk_text_buffer_get_selection_bounds(
            buffer,
            &selection_start,
            &selection_end)) {
        int start_offset =
            gtk_text_iter_get_offset(&selection_start);
        int end_offset =
            gtk_text_iter_get_offset(&selection_end);
        unsigned int difference = start_offset <= end_offset
            ? (unsigned int)(end_offset - start_offset)
            : (unsigned int)(start_offset - end_offset);
        selection_length = (uint32_t)difference;
    }

    path = document_path(&document);
    if (path == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = adapter->context_interaction_sink.editor_location(
        adapter->context_interaction_sink.context,
        view_id,
        path,
        document.language_id,
        line,
        column,
        selection_length,
        now_ms());

    g_free(path);
    return status;
}

static void on_text_mark_set(
    GtkTextBuffer *buffer,
    const GtkTextIter *location,
    GtkTextMark *mark,
    gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *view_id;
    GtkTextMark *insert_mark;
    GtkTextMark *selection_mark;

    (void)location;
    if (adapter == NULL || buffer == NULL || mark == NULL ||
        !adapter->context_interactions_bound ||
        adapter->applying_document_state) {
        return;
    }

    insert_mark = gtk_text_buffer_get_insert(buffer);
    selection_mark = gtk_text_buffer_get_selection_bound(buffer);
    if (mark != insert_mark && mark != selection_mark) return;

    view_id = (const char *)g_object_get_data(
        G_OBJECT(buffer),
        UMI_GTK4_CONTEXT_VIEW_ID_KEY);
    if (view_id == NULL || view_id[0] == '\0') return;

    (void)emit_editor_location(
        adapter,
        buffer,
        view_id);
}

static void bind_editor_page(
    UmiGtk4Adapter *adapter,
    GtkWidget *page)
{
    GtkWidget *text_view;
    GtkTextBuffer *buffer;
    const char *view_id;

    if (adapter == NULL || page == NULL) return;

    view_id = (const char *)g_object_get_data(
        G_OBJECT(page),
        "umicom-view-id");
    if (view_id == NULL || view_id[0] == '\0') return;

    text_view = find_text_view(page);
    if (text_view == NULL) return;
    buffer = gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(text_view));
    if (buffer == NULL) return;

    g_object_set_data_full(
        G_OBJECT(buffer),
        UMI_GTK4_CONTEXT_VIEW_ID_KEY,
        g_strdup(view_id),
        g_free);
    g_object_set_data(
        G_OBJECT(buffer),
        UMI_GTK4_CONTEXT_ADAPTER_KEY,
        adapter);

    if (g_object_get_data(
            G_OBJECT(buffer),
            UMI_GTK4_CONTEXT_BOUND_KEY) == NULL) {
        g_signal_connect(
            buffer,
            "mark-set",
            G_CALLBACK(on_text_mark_set),
            adapter);
        g_object_set_data(
            G_OBJECT(buffer),
            UMI_GTK4_CONTEXT_BOUND_KEY,
            GINT_TO_POINTER(1));
    }
}

static void bind_notebook_pages(
    UmiGtk4Adapter *adapter,
    GtkWidget *notebook)
{
    int page_count;
    int page_index;

    if (adapter == NULL || notebook == NULL ||
        !GTK_IS_NOTEBOOK(notebook)) {
        return;
    }

    page_count = gtk_notebook_get_n_pages(
        GTK_NOTEBOOK(notebook));
    for (page_index = 0;
         page_index < page_count;
         ++page_index) {
        GtkWidget *page = gtk_notebook_get_nth_page(
            GTK_NOTEBOOK(notebook),
            page_index);
        bind_editor_page(adapter, page);
    }
}

static void emit_document_activation(
    UmiGtk4Adapter *adapter,
    GtkWidget *page)
{
    UmiUiDocumentViewSnapshot document;
    GtkWidget *text_view;
    GtkTextBuffer *buffer;
    const char *view_id;
    char *path;

    if (adapter == NULL || page == NULL ||
        !adapter->context_interactions_bound) {
        return;
    }

    view_id = (const char *)g_object_get_data(
        G_OBJECT(page),
        "umicom-view-id");
    if (view_id == NULL || view_id[0] == '\0') return;

    if (find_document(adapter, view_id, &document) != UMI_STATUS_OK) {
        return;
    }
    path = document_path(&document);
    if (path == NULL) return;

    if (adapter->context_interaction_sink.document_activated != NULL) {
        (void)adapter->context_interaction_sink.document_activated(
            adapter->context_interaction_sink.context,
            view_id,
            path,
            document.language_id,
            now_ms());
    }

    text_view = find_text_view(page);
    if (text_view != NULL) {
        buffer = gtk_text_view_get_buffer(
            GTK_TEXT_VIEW(text_view));
        if (buffer != NULL) {
            (void)emit_editor_location(
                adapter,
                buffer,
                view_id);
        }
    }
    g_free(path);
}

static void on_context_document_switched(
    GtkNotebook *notebook,
    GtkWidget *page,
    guint page_num,
    gpointer user_data)
{
    UmiGtk4Adapter *adapter =
        (UmiGtk4Adapter *)user_data;
    (void)notebook;
    (void)page_num;
    if (adapter == NULL || adapter->applying_document_state) return;
    emit_document_activation(adapter, page);
}

static void bind_notebook_switch(
    UmiGtk4Adapter *adapter,
    GtkWidget *notebook,
    gulong *handler_id)
{
    if (adapter == NULL || notebook == NULL ||
        handler_id == NULL || !GTK_IS_NOTEBOOK(notebook)) {
        return;
    }
    if (*handler_id == 0U) {
        *handler_id = g_signal_connect(
            notebook,
            "switch-page",
            G_CALLBACK(on_context_document_switched),
            adapter);
    }
}

void umi_gtk4_context_interaction_tag_problem_list(
    GtkWidget *list)
{
    if (list == NULL) return;
    g_object_set_data_full(
        G_OBJECT(list),
        UMI_GTK4_CONTEXT_SOURCE_KIND_KEY,
        g_strdup("problems"),
        g_free);
}

void umi_gtk4_context_interaction_tag_source_control_row(
    GtkWidget *widget,
    const char *view_kind,
    const char *row_text)
{
    if (widget == NULL || view_kind == NULL ||
        row_text == NULL) {
        return;
    }
    g_object_set_data_full(
        G_OBJECT(widget),
        UMI_GTK4_CONTEXT_SOURCE_KIND_KEY,
        g_strdup("source-control"),
        g_free);
    g_object_set_data_full(
        G_OBJECT(widget),
        UMI_GTK4_CONTEXT_VIEW_KIND_KEY,
        g_strdup(view_kind),
        g_free);
    g_object_set_data_full(
        G_OBJECT(widget),
        UMI_GTK4_CONTEXT_ROW_TEXT_KEY,
        g_strdup(row_text),
        g_free);
}

static void on_problem_row_selected(
    GtkListBox *list,
    GtkListBoxRow *row,
    gpointer user_data)
{
    UmiGtk4Adapter *adapter =
        (UmiGtk4Adapter *)user_data;
    GtkWidget *child;
    const char *text;

    if (adapter == NULL || row == NULL ||
        !adapter->context_interactions_bound ||
        adapter->context_interaction_sink.problem_selected == NULL) {
        return;
    }

    child = gtk_list_box_row_get_child(row);
    if (child == NULL || !GTK_IS_LABEL(child)) return;
    text = gtk_label_get_text(GTK_LABEL(child));
    if (text == NULL || text[0] == '\0') return;

    (void)adapter->context_interaction_sink.problem_selected(
        adapter->context_interaction_sink.context,
        text,
        now_ms());
}

static void on_source_control_pressed(
    GtkGestureClick *gesture,
    int n_press,
    double x,
    double y,
    gpointer user_data)
{
    UmiGtk4Adapter *adapter =
        (UmiGtk4Adapter *)user_data;
    GtkWidget *widget;
    const char *row_text;
    const char *view_kind;

    (void)n_press;
    (void)x;
    (void)y;

    if (adapter == NULL ||
        !adapter->context_interactions_bound ||
        adapter->context_interaction_sink.source_control_selected == NULL) {
        return;
    }

    widget = gtk_event_controller_get_widget(
        GTK_EVENT_CONTROLLER(gesture));
    if (widget == NULL) return;

    row_text = (const char *)g_object_get_data(
        G_OBJECT(widget),
        UMI_GTK4_CONTEXT_ROW_TEXT_KEY);
    view_kind = (const char *)g_object_get_data(
        G_OBJECT(widget),
        UMI_GTK4_CONTEXT_VIEW_KIND_KEY);
    if (row_text == NULL || row_text[0] == '\0') return;

    (void)adapter->context_interaction_sink.source_control_selected(
        adapter->context_interaction_sink.context,
        view_kind != NULL ? view_kind : "source-control",
        row_text,
        now_ms());
}

static void bind_tagged_widgets(
    UmiGtk4Adapter *adapter,
    GtkWidget *widget)
{
    GtkWidget *child;
    const char *source_kind;

    if (adapter == NULL || widget == NULL) return;

    source_kind = (const char *)g_object_get_data(
        G_OBJECT(widget),
        UMI_GTK4_CONTEXT_SOURCE_KIND_KEY);

    if (source_kind != NULL &&
        strcmp(source_kind, "problems") == 0 &&
        GTK_IS_LIST_BOX(widget) &&
        g_object_get_data(
            G_OBJECT(widget),
            UMI_GTK4_CONTEXT_BOUND_KEY) == NULL) {
        g_signal_connect(
            widget,
            "row-selected",
            G_CALLBACK(on_problem_row_selected),
            adapter);
        g_object_set_data(
            G_OBJECT(widget),
            UMI_GTK4_CONTEXT_BOUND_KEY,
            GINT_TO_POINTER(1));
    } else if (source_kind != NULL &&
               strcmp(source_kind, "source-control") == 0 &&
               g_object_get_data(
                   G_OBJECT(widget),
                   UMI_GTK4_CONTEXT_BOUND_KEY) == NULL) {
        GtkGesture *gesture = gtk_gesture_click_new();
        gtk_gesture_single_set_button(
            GTK_GESTURE_SINGLE(gesture),
            GDK_BUTTON_PRIMARY);
        g_signal_connect(
            gesture,
            "pressed",
            G_CALLBACK(on_source_control_pressed),
            adapter);
        gtk_widget_add_controller(
            widget,
            GTK_EVENT_CONTROLLER(gesture));
        g_object_set_data(
            G_OBJECT(widget),
            UMI_GTK4_CONTEXT_BOUND_KEY,
            GINT_TO_POINTER(1));
    }

    child = gtk_widget_get_first_child(widget);
    while (child != NULL) {
        bind_tagged_widgets(adapter, child);
        child = gtk_widget_get_next_sibling(child);
    }
}

UmiStatus umi_gtk4_context_interaction_refresh(
    UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!adapter->context_interactions_bound) {
        return UMI_STATUS_OK;
    }

    bind_notebook_pages(
        adapter,
        adapter->document_notebook);
    bind_notebook_pages(
        adapter,
        adapter->secondary_document_notebook);

    bind_notebook_switch(
        adapter,
        adapter->document_notebook,
        &adapter->context_primary_switch_handler);
    bind_notebook_switch(
        adapter,
        adapter->secondary_document_notebook,
        &adapter->context_secondary_switch_handler);

    if (adapter->window != NULL) {
        bind_tagged_widgets(
            adapter,
            GTK_WIDGET(adapter->window));
    }
    return UMI_STATUS_OK;
}

void umi_gtk4_context_interaction_unbind(
    UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return;

    /*
     * Buffer/row closures intentionally remain connected to their GTK
     * lifetimes. They all test context_interactions_bound before dispatching,
     * so clearing the sink is sufficient and avoids retaining native child
     * pointers that may already have been replaced during a workbench refresh.
     */
    adapter->context_interactions_bound = 0;
    memset(
        &adapter->context_interaction_sink,
        0,
        sizeof(adapter->context_interaction_sink));
}
