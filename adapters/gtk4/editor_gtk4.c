/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/editor_gtk4.c
 *
 * PURPOSE:
 *   Render Framework document views as usable GTK4 text editors, optionally
 *   using GtkSourceView 5 when the dependency is available.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"

#include <string.h>

#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
#include <gtksourceview/gtksource.h>
#endif

typedef struct UmiGtk4EditorBinding {
    UmiGtk4Adapter *adapter;
    char view_id[UMI_UI_ID_CAPACITY];
} UmiGtk4EditorBinding;

static void on_document_page_switched(GtkNotebook *notebook,
                                      GtkWidget *page,
                                      guint page_number,
                                      gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *view_id;
    UmiUiWorkbench *workbench;
    (void)notebook;
    (void)page_number;
    if (adapter == NULL || adapter->shell == NULL || page == NULL) return;
    view_id = (const char *)g_object_get_data(G_OBJECT(page), "umicom-view-id");
    if (view_id == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    (void)umi_ui_workbench_activate_document(workbench, view_id);
}

static void editor_binding_free(gpointer data, GClosure *closure)
{
    /* GClosureNotify supplies the owning closure for advanced finalisers.
     * This binding owns no closure resources, but retaining the exact GTK
     * callback signature keeps strict C23 builds type-safe on Windows. */
    (void)closure;
    g_free(data);
}

static void on_editor_buffer_changed(GtkTextBuffer *text_buffer,
                                     gpointer user_data)
{
    UmiGtk4EditorBinding *binding = (UmiGtk4EditorBinding *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiDocumentViewSnapshot document;
    GtkTextIter start;
    GtkTextIter end;
    char *text;

    if (binding == NULL || binding->adapter == NULL ||
        binding->adapter->shell == NULL) return;
    workbench = umi_ui_application_shell_workbench(binding->adapter->shell);
    if (umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),
                                         binding->view_id,
                                         &document) != UMI_STATUS_OK) return;
    gtk_text_buffer_get_bounds(text_buffer, &start, &end);
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, TRUE);
    if (text == NULL) return;
    (void)g_strlcpy(document.source_text, text, sizeof(document.source_text));
    document.dirty = 1;
    (void)umi_ui_document_view_model_upsert(
        umi_ui_workbench_documents(workbench), &document);
    gtk_label_set_text(GTK_LABEL(binding->adapter->status_label),
                       "Modified — use File / Save to persist the document");
    g_free(text);
}

static GtkWidget *create_editor_widget(const UmiUiDocumentViewSnapshot *document,
                                       GtkTextBuffer **out_buffer)
{
    GtkWidget *view;
#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
    GtkSourceBuffer *source_buffer = gtk_source_buffer_new(NULL);
    GtkSourceLanguageManager *languages = gtk_source_language_manager_get_default();
    GtkSourceLanguage *language = NULL;
    if (document->language_id[0] != '\0') {
        language = gtk_source_language_manager_get_language(
            languages, document->language_id);
    }
    if (language != NULL) gtk_source_buffer_set_language(source_buffer, language);
    gtk_source_buffer_set_highlight_syntax(source_buffer, TRUE);
    view = gtk_source_view_new_with_buffer(source_buffer);
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_indent_on_tab(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(view), 4U);
    *out_buffer = GTK_TEXT_BUFFER(source_buffer);
    g_object_unref(source_buffer);
#else
    view = gtk_text_view_new();
    *out_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
#endif
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_NONE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 8);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(view), 8);
    gtk_widget_add_css_class(view, "umicom-editor");
    return view;
}

UmiStatus umi_gtk4_refresh_documents(UmiGtk4Adapter *adapter,
                                      UmiUiWorkbench *workbench)
{
    UmiUiDocumentViewModel *documents;
    int pages;
    size_t index;

    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(adapter->document_notebook));
    while (pages > 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(adapter->document_notebook), 0);
        pages -= 1;
    }
    documents = umi_ui_workbench_documents(workbench);
    /* Store and disconnect the signal ID rather than using GLib's callback
     * matching convenience macro. That macro passes a function pointer through
     * gpointer, which ISO C correctly diagnoses under -Wpedantic because data
     * and function pointers are distinct types. */
    if (adapter->document_page_switch_handler != 0UL) {
        g_signal_handler_disconnect(adapter->document_notebook,
                                    adapter->document_page_switch_handler);
        adapter->document_page_switch_handler = 0UL;
    }
    for (index = 0U; index < umi_ui_document_view_model_count(documents); ++index) {
        UmiUiDocumentViewSnapshot document;
        if (umi_ui_document_view_model_at(documents, index, &document) == UMI_STATUS_OK) {
            GtkTextBuffer *text_buffer = NULL;
            GtkWidget *view = create_editor_widget(&document, &text_buffer);
            GtkWidget *scroll = gtk_scrolled_window_new();
            GtkWidget *tab_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
            GtkWidget *tab = gtk_label_new(document.title);
            UmiGtk4EditorBinding *binding = g_new0(UmiGtk4EditorBinding, 1);
            int page_index;

            gtk_widget_add_css_class(scroll, "umicom-editor-scroll");
            gtk_widget_add_css_class(tab_box, "umicom-document-tab");
            if (document.dirty) {
                GtkWidget *dirty = gtk_label_new("●");
                gtk_widget_add_css_class(dirty, "accent");
                gtk_box_append(GTK_BOX(tab_box), dirty);
            }
            gtk_box_append(GTK_BOX(tab_box), tab);
            gtk_text_buffer_set_text(text_buffer, document.source_text, -1);
            {
                GtkTextIter cursor;
                GtkTextIter selection_end;
                size_t text_length = strlen(document.source_text);
                size_t cursor_offset = document.cursor_offset <= text_length
                    ? document.cursor_offset : text_length;
                size_t selection_length = document.selection_length <=
                    text_length - cursor_offset
                    ? document.selection_length : text_length - cursor_offset;
                gtk_text_buffer_get_iter_at_offset(text_buffer, &cursor,
                                                   (int)cursor_offset);
                gtk_text_buffer_get_iter_at_offset(text_buffer, &selection_end,
                                                   (int)(cursor_offset + selection_length));
                gtk_text_buffer_select_range(text_buffer, &cursor, &selection_end);
            }
            gtk_text_view_set_editable(GTK_TEXT_VIEW(view), TRUE);
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                           GTK_POLICY_AUTOMATIC,
                                           GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
            binding->adapter = adapter;
            (void)g_strlcpy(binding->view_id,
                            document.view_id,
                            sizeof(binding->view_id));
            g_signal_connect_data(text_buffer,
                                  "changed",
                                  G_CALLBACK(on_editor_buffer_changed),
                                  binding,
                                  editor_binding_free,
                                  0);
            page_index = gtk_notebook_append_page(
                GTK_NOTEBOOK(adapter->document_notebook),
                scroll,
                tab_box);
            gtk_notebook_set_tab_reorderable(
                GTK_NOTEBOOK(adapter->document_notebook),
                scroll,
                TRUE);
            g_object_set_data_full(G_OBJECT(scroll), "umicom-view-id",
                                   g_strdup(document.view_id), g_free);
            gtk_widget_set_tooltip_text(tab_box,
                document.uri[0] != '\0' ? document.uri : document.document_id);
            if (document.active) {
                gtk_notebook_set_current_page(GTK_NOTEBOOK(adapter->document_notebook),
                                              page_index);
            }
        }
    }
    adapter->document_page_switch_handler =
        g_signal_connect(adapter->document_notebook,
                         "switch-page",
                         G_CALLBACK(on_document_page_switched),
                         adapter);
    return UMI_STATUS_OK;
}
