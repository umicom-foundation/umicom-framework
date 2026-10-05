/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_typing_history_gtk4.c
 * PURPOSE: Exercise real GTK typing groups and restored selection through the shared document history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/document_commands.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #c);                                                       \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
static GtkWidget *FindEditor(GtkWidget *root)
{
    if (GTK_IS_TEXT_VIEW(root) && gtk_widget_has_css_class(root, "umicom-editor"))
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = FindEditor(child);
        if (found != NULL)
            return found;
    }
    return NULL;
}
static int NativeText(GtkTextBuffer *buffer, const char *expected)
{
    GtkTextIter a, b;
    gtk_text_buffer_get_bounds(buffer, &a, &b);
    char *text = gtk_text_buffer_get_text(buffer, &a, &b, TRUE);
    int equal = strcmp(text, expected) == 0;
    g_free(text);
    return equal;
}
static int ModelText(UmiUiDocumentViewModel *views, const char *id, const char *expected)
{
    char *text = NULL;
    size_t bytes = 0U;
    UmiStatus status = UmiUiDocumentViewModelCopyText(views, id, &text, &bytes);
    int equal = status == UMI_STATUS_OK && bytes == strlen(expected) && strcmp(text, expected) == 0;
    UmiUiDocumentViewModelFreeText(text);
    return equal;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1],
               *cases[] = {"insert",      "selection", "reverse-selection", "unicode",
                           "crlf",        "empty",     "unchanged",         "nested",
                           "two-actions", "detach",    "destroy",           "retained-buffer"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    GtkApplication *application = NULL;
    GtkTextBuffer *buffer = NULL;
    application = gtk_application_new("org.umicom.typing-history-test", G_APPLICATION_NON_UNIQUE);
    CHECK(application != NULL && g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("typing-history.gtk", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.typing-history-test", "Typing history", workbench,
                                          &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    char id[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_new(documents, "draft.c", id, sizeof(id)) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, id, &view) == UMI_STATUS_OK);
    const char *source = "ab\ncd", *expected = "abX\ncd";
    int first = 2, last = 2;
    size_t byte_first = 2U, selected = 0U;
    if (strcmp(name, "selection") == 0 || strcmp(name, "reverse-selection") == 0)
    {
        first = 1;
        last = 4;
        byte_first = 1U;
        selected = 3U;
        expected = "aXd";
    }
    if (strcmp(name, "unicode") == 0)
    {
        source = "\xe9\x9b\xaa"
                 "ab";
        expected = "\xe9\x9b\xaa"
                   "Xb";
        first = 1;
        last = 2;
        byte_first = 3U;
        selected = 1U;
    }
    if (strcmp(name, "crlf") == 0)
    {
        source = "a\r\nb";
        expected = "aXb";
        first = 1;
        last = 3;
        byte_first = 1U;
        selected = 2U;
    }
    if (strcmp(name, "empty") == 0)
    {
        source = "";
        expected = "X";
        first = last = 0;
        byte_first = 0U;
    }
    if (strcmp(name, "unchanged") == 0)
        expected = source;
    view.cursor_offset = byte_first;
    view.selection_length = selected;
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, source, strlen(source)) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_sync_active(documents) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK &&
          umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, NULL, NULL) == UMI_STATUS_OK);
    GtkWidget *editor = FindEditor(umi_gtk4_adapter_native_window(adapter));
    CHECK(editor != NULL);
    buffer = g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor)));
    GtkTextIter a, b;
    gtk_text_buffer_get_iter_at_offset(buffer, &a, first);
    gtk_text_buffer_get_iter_at_offset(buffer, &b, last);
    gtk_text_buffer_select_range(buffer, strcmp(name, "reverse-selection") == 0 ? &b : &a,
                                 strcmp(name, "reverse-selection") == 0 ? &a : &b);
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(documents, &before) == UMI_STATUS_OK);
    gtk_text_buffer_begin_user_action(buffer);
    if (strcmp(name, "nested") == 0)
        gtk_text_buffer_begin_user_action(buffer);
    if (strcmp(name, "unchanged") != 0)
    {
        if (first != last)
            gtk_text_buffer_delete(buffer, &a, &b);
        gtk_text_buffer_insert(buffer, &a, "X", 1);
        gtk_text_buffer_place_cursor(buffer, &a);
    }
    if (strcmp(name, "detach") == 0)
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
    if (strcmp(name, "destroy") == 0)
    {
        umi_gtk4_adapter_destroy(adapter);
        adapter = NULL;
    }
    if (strcmp(name, "nested") == 0)
        gtk_text_buffer_end_user_action(buffer);
    gtk_text_buffer_end_user_action(buffer);
    CHECK(NativeText(buffer, expected) && ModelText(views, id, expected));
    if (strcmp(name, "detach") == 0 || strcmp(name, "destroy") == 0)
        goto cleanup;
    CHECK(umi_document_coordinator_active_snapshot(documents, &after) == UMI_STATUS_OK);
    CHECK(after.undo_count == before.undo_count + (strcmp(name, "unchanged") == 0 ? 0U : 1U));
    if (strcmp(name, "unchanged") == 0)
        goto cleanup;
    CHECK(umi_ui_document_view_model_find(views, id, &view) == UMI_STATUS_OK);
    size_t typed_cursor = view.cursor_offset;
    if (strcmp(name, "two-actions") == 0)
    {
        gtk_text_buffer_get_end_iter(buffer, &a);
        gtk_text_buffer_place_cursor(buffer, &a);
        gtk_text_buffer_begin_user_action(buffer);
        gtk_text_buffer_insert(buffer, &a, "Y", 1);
        gtk_text_buffer_end_user_action(buffer);
        CHECK(UmiGtk4AdapterDocumentCommand(adapter, "edit.undo") == UMI_STATUS_OK &&
              NativeText(buffer, expected));
        /* Redo of the first action returns to the location left immediately
         * before its Undo, here the end of the first action's text. */
        typed_cursor = strlen(expected);
    }
    CHECK(UmiGtk4AdapterDocumentCommand(adapter, "edit.undo") == UMI_STATUS_OK);
    CHECK(NativeText(buffer, source) && ModelText(views, id, source));
    CHECK(umi_ui_document_view_model_find(views, id, &view) == UMI_STATUS_OK &&
          view.cursor_offset == byte_first && view.selection_length == selected);
    GtkTextIter insert, bound;
    gtk_text_buffer_get_iter_at_mark(buffer, &insert, gtk_text_buffer_get_insert(buffer));
    gtk_text_buffer_get_iter_at_mark(buffer, &bound, gtk_text_buffer_get_selection_bound(buffer));
    CHECK(MIN(gtk_text_iter_get_offset(&insert), gtk_text_iter_get_offset(&bound)) == first &&
          MAX(gtk_text_iter_get_offset(&insert), gtk_text_iter_get_offset(&bound)) == last);
    CHECK(UmiGtk4AdapterDocumentCommand(adapter, "edit.redo") == UMI_STATUS_OK &&
          NativeText(buffer, expected));
    CHECK(umi_ui_document_view_model_find(views, id, &view) == UMI_STATUS_OK &&
          view.cursor_offset == typed_cursor && view.selection_length == 0U);
    if (strcmp(name, "retained-buffer") == 0)
    {
        umi_gtk4_adapter_destroy(adapter);
        adapter = NULL;
        gtk_text_buffer_begin_user_action(buffer);
        gtk_text_buffer_get_end_iter(buffer, &a);
        gtk_text_buffer_insert(buffer, &a, "late", 4);
        gtk_text_buffer_end_user_action(buffer);
        CHECK(ModelText(views, id, expected));
    }
cleanup:
    if (adapter != NULL)
    {
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
        umi_gtk4_adapter_destroy(adapter);
    }
    g_clear_object(&buffer);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    return failed;
}
