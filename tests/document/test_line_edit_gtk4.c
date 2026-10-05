/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_line_edit_gtk4.c
 * PURPOSE: Check native line-edit results, shared history and completion callbacks that detach or destroy the host.
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
typedef struct Completion
{
    UmiGtk4Adapter *adapter;
    unsigned calls;
    UmiStatus status;
    int detach, destroy;
} Completion;
static void Completed(void *context, UmiStatus status)
{
    Completion *result = context;
    ++result->calls;
    result->status = status;
    if (result->detach)
        (void)UmiGtk4AdapterBindDocumentEditing(result->adapter, NULL, NULL, NULL);
    if (result->destroy)
    {
        umi_gtk4_adapter_destroy(result->adapter);
        result->adapter = NULL;
    }
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1], *cases[] = {"duplicate",
                                            "delete",
                                            "move-up",
                                            "move-down",
                                            "join",
                                            "indent",
                                            "outdent",
                                            "comment",
                                            "trim",
                                            "selection",
                                            "unicode",
                                            "crlf",
                                            "read-only",
                                            "invalid-kind",
                                            "no-document",
                                            "no-neighbor",
                                            "no-change",
                                            "unbound",
                                            "null-adapter",
                                            "completion-detach",
                                            "completion-destroy"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    Completion result = {0};
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    GtkApplication *application = NULL;
    char *text = NULL;
    size_t bytes = 0U;
    application = gtk_application_new("org.umicom.line-edit-test", G_APPLICATION_NON_UNIQUE);
    CHECK(application != NULL && g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("line-edit.gtk", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.line-edit-test", "Line editing", workbench, &shell) ==
          UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    char id[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_new(documents, "draft.c", id, sizeof(id)) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, id, &view) == UMI_STATUS_OK);
    const char *source = "a\nbb\nccc", *expected = "a\nbb\nbb\nccc";
    view.cursor_offset = 2U;
    view.selection_length = 0U;
    view.dirty = 1;
    UmiEditorEditCommandKind kind = UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE;
    UmiStatus wanted = UMI_STATUS_OK;
    int changed = 1, notify = 1;
    if (strcmp(name, "delete") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_DELETE_LINE;
        expected = "a\nccc";
    }
    if (strcmp(name, "move-up") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP;
        expected = "bb\na\nccc";
    }
    if (strcmp(name, "move-down") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN;
        expected = "a\nccc\nbb";
    }
    if (strcmp(name, "join") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT;
        expected = "a\nbb ccc";
    }
    if (strcmp(name, "indent") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_INDENT_LINES;
        expected = "a\n    bb\nccc";
    }
    if (strcmp(name, "outdent") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES;
        source = "a\n  bb\nccc";
        expected = "a\nbb\nccc";
    }
    if (strcmp(name, "comment") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT;
        expected = "a\n// bb\nccc";
    }
    if (strcmp(name, "trim") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE;
        source = "a \nbb\t\nccc ";
        view.cursor_offset = 3U;
        expected = "a\nbb\nccc";
    }
    if (strcmp(name, "selection") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_INDENT_LINES;
        view.cursor_offset = 0U;
        view.selection_length = 5U;
        expected = "    a\n    bb\nccc";
    }
    if (strcmp(name, "unicode") == 0)
    {
        source = "a\n\xe9\x9b\xaa";
        expected = "a\n\xe9\x9b\xaa\n\xe9\x9b\xaa";
    }
    if (strcmp(name, "crlf") == 0)
    {
        source = "a\r\nbb";
        view.cursor_offset = 3U;
        expected = "a\r\nbb\r\nbb";
    }
    if (strcmp(name, "read-only") == 0)
    {
        view.read_only = 1;
        wanted = UMI_STATUS_PERMISSION_DENIED;
    }
    if (strcmp(name, "invalid-kind") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "no-neighbor") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP;
        view.cursor_offset = 0U;
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(name, "no-change") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES;
        expected = source;
        changed = 0;
    }
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, source, strlen(source)) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_sync_active(documents) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &result.adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(result.adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(result.adapter, documents, Completed, &result) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(documents, &before) == UMI_STATUS_OK);
    if (strcmp(name, "no-document") == 0)
    {
        CHECK(umi_document_coordinator_close_active(documents, 1) == UMI_STATUS_OK);
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(name, "unbound") == 0)
    {
        CHECK(UmiGtk4AdapterBindDocumentEditing(result.adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        wanted = UMI_STATUS_UNAVAILABLE;
        notify = 0;
    }
    if (strcmp(name, "null-adapter") == 0)
    {
        wanted = UMI_STATUS_INVALID_ARGUMENT;
        notify = 0;
    }
    result.detach = strcmp(name, "completion-detach") == 0;
    result.destroy = strcmp(name, "completion-destroy") == 0;
    CHECK(UmiGtk4AdapterEditLines(strcmp(name, "null-adapter") == 0 ? NULL : result.adapter, kind, NULL) ==
          wanted);
    CHECK(result.calls == (unsigned)notify && (!notify || result.status == wanted));
    if (result.detach)
        CHECK(!UmiGtk4AdapterDocumentHasCompletion(result.adapter));
    if (result.destroy)
        CHECK(result.adapter == NULL);
    if (strcmp(name, "no-document") != 0)
    {
        CHECK(UmiUiDocumentViewModelCopyText(views, id, &text, &bytes) == UMI_STATUS_OK);
        CHECK(strcmp(text, wanted == UMI_STATUS_OK ? expected : source) == 0);
        CHECK(umi_document_coordinator_active_snapshot(documents, &after) == UMI_STATUS_OK);
        CHECK(after.undo_count == before.undo_count + ((wanted == UMI_STATUS_OK && changed) ? 1U : 0U));
        if (wanted == UMI_STATUS_OK && changed)
        {
            CHECK(umi_document_coordinator_undo(documents) == UMI_STATUS_OK);
            UmiUiDocumentViewModelFreeText(text);
            text = NULL;
            CHECK(UmiUiDocumentViewModelCopyText(views, id, &text, &bytes) == UMI_STATUS_OK &&
                  strcmp(text, source) == 0);
        }
    }
cleanup:
    UmiUiDocumentViewModelFreeText(text);
    if (result.adapter != NULL)
    {
        (void)UmiGtk4AdapterBindDocumentEditing(result.adapter, NULL, NULL, NULL);
        umi_gtk4_adapter_destroy(result.adapter);
    }
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    return failed;
}
