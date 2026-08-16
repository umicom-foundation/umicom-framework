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

#include "umicom/editor/presentation.h"

#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
#include <gtksourceview/gtksource.h>
#endif

typedef struct UmiGtk4EditorBinding {
    UmiGtk4Adapter *adapter;
    char view_id[UMI_UI_ID_CAPACITY];
} UmiGtk4EditorBinding;

static const char *effective_editor_group(
    const UmiUiDocumentViewSnapshot *document)
{
    return document->group_id[0] != '\0'
        ? document->group_id
        : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
}

static GtkWidget *notebook_for_group(UmiGtk4Adapter *adapter,
                                     const char *group_id)
{
    return strcmp(group_id, UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0
        ? adapter->secondary_document_notebook
        : adapter->document_notebook;
}

static void clear_notebook(GtkWidget *notebook)
{
    int pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    while (pages > 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), 0);
        --pages;
    }
}

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

static const char *group_for_notebook(UmiGtk4Adapter *adapter,
                                      GtkNotebook *notebook)
{
    return GTK_WIDGET(notebook) == adapter->secondary_document_notebook
        ? UMI_UI_SECONDARY_EDITOR_GROUP_ID
        : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
}

static void synchronise_document_page(GtkNotebook *notebook,
                                      GtkWidget *page,
                                      guint page_number,
                                      gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    UmiUiWorkbench *workbench;
    const char *view_id;
    const char *group_id;
    UmiStatus status;

    if (adapter == NULL || adapter->shell == NULL || page == NULL ||
        adapter->applying_document_state) {
        return;
    }
    view_id = (const char *)g_object_get_data(G_OBJECT(page),
                                              "umicom-view-id");
    if (view_id == NULL) return;
    group_id = group_for_notebook(adapter, notebook);
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    status = umi_ui_document_view_model_place(
        umi_ui_workbench_documents(workbench), view_id, group_id,
        (size_t)page_number);
    if (status == UMI_STATUS_OK) {
        (void)umi_ui_workbench_activate_document(workbench, view_id);
        gtk_label_set_text(GTK_LABEL(adapter->status_label),
                           "Editor layout updated");
    }
}

static void on_document_page_added(GtkNotebook *notebook,
                                   GtkWidget *page,
                                   guint page_number,
                                   gpointer user_data)
{
    synchronise_document_page(notebook, page, page_number, user_data);
}

static void on_document_page_reordered(GtkNotebook *notebook,
                                       GtkWidget *page,
                                       guint page_number,
                                       gpointer user_data)
{
    synchronise_document_page(notebook, page, page_number, user_data);
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
    document.preview = 0;
    (void)umi_ui_document_view_model_upsert(
        umi_ui_workbench_documents(workbench), &document);
    gtk_label_set_text(GTK_LABEL(binding->adapter->status_label),
                       "Modified — use File / Save to persist the document");
    g_free(text);
}

static UmiGtk4EditorBinding *editor_binding_new(UmiGtk4Adapter *adapter,
                                                const char *view_id)
{
    UmiGtk4EditorBinding *binding = g_new0(UmiGtk4EditorBinding, 1);
    binding->adapter = adapter;
    (void)g_strlcpy(binding->view_id,
                    view_id,
                    sizeof(binding->view_id));
    return binding;
}

static void on_editor_close_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4EditorBinding *binding = (UmiGtk4EditorBinding *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiWorkbenchSnapshot workbench_snapshot;
    UmiUiDocumentViewSnapshot document;
    UmiStatus status;
    (void)button;
    if (binding == NULL || binding->adapter == NULL ||
        binding->adapter->shell == NULL) return;
    workbench = umi_ui_application_shell_workbench(binding->adapter->shell);
    status = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(workbench), binding->view_id, &document);
    if (status != UMI_STATUS_OK) return;
    if (document.dirty) {
        gtk_label_set_text(GTK_LABEL(binding->adapter->status_label),
                           "Save or revert the modified editor before closing it");
        return;
    }
    if (document.pinned) {
        gtk_label_set_text(GTK_LABEL(binding->adapter->status_label),
                           "Unpin the editor before closing it");
        return;
    }
    status = umi_ui_workbench_snapshot(workbench, &workbench_snapshot);
    if (status != UMI_STATUS_OK) return;
    status = umi_ui_document_view_model_remove(
        umi_ui_workbench_documents(workbench), binding->view_id);
    if (status == UMI_STATUS_OK) {
        if (strcmp(workbench_snapshot.active_document_view,
                   binding->view_id) == 0) {
            char next_view_id[UMI_UI_ID_CAPACITY];
            const char *closed_group = effective_editor_group(&document);
            status = umi_ui_document_view_model_activate_group(
                umi_ui_workbench_documents(workbench), closed_group,
                next_view_id, sizeof(next_view_id));
            if (status == UMI_STATUS_NOT_FOUND) {
                const char *other_group = strcmp(
                    closed_group, UMI_UI_PRIMARY_EDITOR_GROUP_ID) == 0
                    ? UMI_UI_SECONDARY_EDITOR_GROUP_ID
                    : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
                status = umi_ui_document_view_model_activate_group(
                    umi_ui_workbench_documents(workbench), other_group,
                    next_view_id, sizeof(next_view_id));
            }
            if (status == UMI_STATUS_OK) {
                (void)umi_ui_workbench_activate_document(workbench,
                                                         next_view_id);
            }
        }
        (void)umi_gtk4_refresh_documents(binding->adapter, workbench);
    }
}

static void on_editor_pin_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4EditorBinding *binding = (UmiGtk4EditorBinding *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiDocumentViewSnapshot document;
    UmiStatus status;
    (void)button;
    if (binding == NULL || binding->adapter == NULL ||
        binding->adapter->shell == NULL) return;
    workbench = umi_ui_application_shell_workbench(binding->adapter->shell);
    status = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(workbench), binding->view_id, &document);
    if (status != UMI_STATUS_OK) return;
    status = umi_ui_document_view_model_set_pinned(
        umi_ui_workbench_documents(workbench),
        binding->view_id,
        !document.pinned);
    if (status == UMI_STATUS_OK) {
        (void)umi_gtk4_refresh_documents(binding->adapter, workbench);
    }
}

static GtkWidget *create_editor_widget(UmiUiWorkbench *workbench,
                                       const UmiUiDocumentViewSnapshot *document,
                                       GtkTextBuffer **out_buffer)
{
    GtkWidget *view;
    UmiEditorPresentationSnapshot presentation;
    UmiStatus presentation_status = umi_editor_presentation_default(
        document->language_id, &presentation);
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
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view),
                                          document->show_line_numbers != 0);
    gtk_source_view_set_auto_indent(
        GTK_SOURCE_VIEW(view), presentation_status == UMI_STATUS_OK
            ? presentation.auto_indent != 0 : TRUE);
    gtk_source_view_set_indent_on_tab(
        GTK_SOURCE_VIEW(view), presentation_status == UMI_STATUS_OK
            ? presentation.indent_on_tab != 0 : TRUE);
    gtk_source_view_set_tab_width(
        GTK_SOURCE_VIEW(view), presentation_status == UMI_STATUS_OK
            ? presentation.tab_width : 4U);
    if (presentation_status == UMI_STATUS_OK) {
        gtk_source_buffer_set_highlight_matching_brackets(
            source_buffer, presentation.highlight_matching_brackets != 0);
        gtk_source_view_set_right_margin_position(
            GTK_SOURCE_VIEW(view), presentation.right_margin_column);
        gtk_source_view_set_show_right_margin(
            GTK_SOURCE_VIEW(view), presentation.show_right_margin != 0);
    }
    *out_buffer = GTK_TEXT_BUFFER(source_buffer);
    g_object_unref(source_buffer);
#else
    (void)presentation_status;
    (void)presentation;
    view = gtk_text_view_new();
    *out_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
#endif
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),
                                document->word_wrap
                                    ? GTK_WRAP_WORD_CHAR
                                    : GTK_WRAP_NONE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 8);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(view), 8);
    gtk_widget_add_css_class(view, "umicom-editor");
    (void)umi_gtk4_configure_editor_theme(view, workbench);
    return view;
}

UmiStatus umi_gtk4_refresh_documents(UmiGtk4Adapter *adapter,
                                      UmiUiWorkbench *workbench)
{
    UmiUiDocumentViewModel *documents;
    UmiUiWorkbenchState state;
    size_t index;

    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    adapter->applying_document_state = 1;
    clear_notebook(adapter->document_notebook);
    clear_notebook(adapter->secondary_document_notebook);
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
    if (adapter->secondary_document_page_switch_handler != 0UL) {
        g_signal_handler_disconnect(
            adapter->secondary_document_notebook,
            adapter->secondary_document_page_switch_handler);
        adapter->secondary_document_page_switch_handler = 0UL;
    }
    (void)umi_ui_workbench_state_snapshot(workbench, &state);
    if (strcmp(state.active_editor_group,
               UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0) {
        gtk_widget_remove_css_class(adapter->document_notebook, "active");
        gtk_widget_add_css_class(adapter->secondary_document_notebook,
                                 "active");
    } else {
        gtk_widget_add_css_class(adapter->document_notebook, "active");
        gtk_widget_remove_css_class(adapter->secondary_document_notebook,
                                    "active");
    }
    for (index = 0U; index < umi_ui_document_view_model_count(documents); ++index) {
        UmiUiDocumentViewSnapshot document;
        if (umi_ui_document_view_model_at(documents, index, &document) == UMI_STATUS_OK) {
            GtkTextBuffer *text_buffer = NULL;
            GtkWidget *view = create_editor_widget(workbench, &document,
                                                   &text_buffer);
            GtkWidget *scroll = gtk_scrolled_window_new();
            GtkWidget *tab_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
            GtkWidget *tab = gtk_label_new(document.title);
            UmiGtk4EditorBinding *binding =
                editor_binding_new(adapter, document.view_id);
            GtkWidget *notebook = notebook_for_group(
                adapter, effective_editor_group(&document));
            int page_index;

            gtk_widget_add_css_class(scroll, "umicom-editor-scroll");
            gtk_widget_add_css_class(tab_box, "umicom-document-tab");
            if (document.icon_name[0] != '\0') {
                GtkWidget *icon = gtk_image_new_from_icon_name(document.icon_name);
                gtk_widget_add_css_class(icon, "umicom-document-icon");
                gtk_box_append(GTK_BOX(tab_box), icon);
            }
            if (document.dirty) {
                GtkWidget *dirty = gtk_label_new("●");
                gtk_widget_add_css_class(dirty, "accent");
                gtk_widget_set_tooltip_text(dirty, "Modified — not yet saved");
                gtk_box_append(GTK_BOX(tab_box), dirty);
            }
            if (document.preview) {
                gtk_widget_add_css_class(tab_box, "preview");
            }
            gtk_box_append(GTK_BOX(tab_box), tab);
            if (document.read_only) {
                GtkWidget *read_only =
                    gtk_image_new_from_icon_name("changes-prevent-symbolic");
                gtk_widget_add_css_class(read_only, "umicom-document-state");
                gtk_widget_set_tooltip_text(read_only, "Read-only editor");
                gtk_box_append(GTK_BOX(tab_box), read_only);
            }
            {
                GtkWidget *pin = gtk_button_new_from_icon_name(
                    document.pinned
                        ? "view-pin-symbolic"
                        : "view-pin-outline-symbolic");
                UmiGtk4EditorBinding *pin_binding =
                    editor_binding_new(adapter, document.view_id);
                gtk_widget_add_css_class(pin, "umicom-document-tab-button");
                gtk_widget_set_can_focus(pin, FALSE);
                gtk_widget_set_tooltip_text(
                    pin, document.pinned ? "Unpin editor" : "Pin editor");
                g_signal_connect_data(pin,
                                      "clicked",
                                      G_CALLBACK(on_editor_pin_clicked),
                                      pin_binding,
                                      editor_binding_free,
                                      0);
                gtk_box_append(GTK_BOX(tab_box), pin);
            }
            if (document.closable) {
                GtkWidget *close =
                    gtk_button_new_from_icon_name("window-close-symbolic");
                UmiGtk4EditorBinding *close_binding =
                    editor_binding_new(adapter, document.view_id);
                gtk_widget_add_css_class(close, "umicom-document-tab-button");
                gtk_widget_set_can_focus(close, FALSE);
                gtk_widget_set_tooltip_text(close, "Close editor");
                g_signal_connect_data(close,
                                      "clicked",
                                      G_CALLBACK(on_editor_close_clicked),
                                      close_binding,
                                      editor_binding_free,
                                      0);
                gtk_box_append(GTK_BOX(tab_box), close);
            }
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
            gtk_text_view_set_editable(GTK_TEXT_VIEW(view),
                                       !document.read_only);
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                           GTK_POLICY_AUTOMATIC,
                                           GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
            g_signal_connect_data(text_buffer,
                                  "changed",
                                  G_CALLBACK(on_editor_buffer_changed),
                                  binding,
                                  editor_binding_free,
                                  0);
            page_index = gtk_notebook_append_page(
                GTK_NOTEBOOK(notebook),
                scroll,
                tab_box);
            gtk_notebook_set_tab_reorderable(
                GTK_NOTEBOOK(notebook),
                scroll,
                TRUE);
            gtk_notebook_set_tab_detachable(
                GTK_NOTEBOOK(notebook),
                scroll,
                TRUE);
            g_object_set_data_full(G_OBJECT(scroll), "umicom-view-id",
                                   g_strdup(document.view_id), g_free);
            gtk_widget_set_tooltip_text(tab_box,
                document.uri[0] != '\0' ? document.uri : document.document_id);
            if (document.active) {
                gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook),
                                              page_index);
            }
        }
    }
    adapter->document_page_switch_handler =
        g_signal_connect(adapter->document_notebook,
                         "switch-page",
                         G_CALLBACK(on_document_page_switched),
                         adapter);
    adapter->secondary_document_page_switch_handler =
        g_signal_connect(adapter->secondary_document_notebook,
                         "switch-page",
                         G_CALLBACK(on_document_page_switched),
                         adapter);
    if (adapter->document_page_added_handler == 0UL) {
        adapter->document_page_added_handler =
            g_signal_connect(adapter->document_notebook,
                             "page-added",
                             G_CALLBACK(on_document_page_added),
                             adapter);
    }
    if (adapter->document_page_reordered_handler == 0UL) {
        adapter->document_page_reordered_handler =
            g_signal_connect(adapter->document_notebook,
                             "page-reordered",
                             G_CALLBACK(on_document_page_reordered),
                             adapter);
    }
    if (adapter->secondary_document_page_added_handler == 0UL) {
        adapter->secondary_document_page_added_handler =
            g_signal_connect(adapter->secondary_document_notebook,
                             "page-added",
                             G_CALLBACK(on_document_page_added),
                             adapter);
    }
    if (adapter->secondary_document_page_reordered_handler == 0UL) {
        adapter->secondary_document_page_reordered_handler =
            g_signal_connect(adapter->secondary_document_notebook,
                             "page-reordered",
                             G_CALLBACK(on_document_page_reordered),
                             adapter);
    }
    adapter->applying_document_state = 0;
    return UMI_STATUS_OK;
}
