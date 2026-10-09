/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_format_gtk4.c
 * PURPOSE: Exercise shared native format commands, source history and callbacks which retire their host.
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
    const char *name = argv[1],
               *cases[] = {
                   "lf",       "crlf",         "encoding-only", "read-only",         "no-document",
                   "unbound",  "null-adapter", "parent-close",  "completion-detach", "completion-destroy",
                   "unchanged"};
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
    application = gtk_application_new("org.umicom.document-format-test", G_APPLICATION_NON_UNIQUE);
    CHECK(application != NULL && g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("document-format.gtk", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.document-format-test", "Document format", workbench,
                                          &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    char id[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_new(documents, "draft.c", id, sizeof(id)) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, id, &view) == UMI_STATUS_OK);

    const char *source = "a\nb\n", *expected = "a\r\nb\r\n";
    UmiDocumentFormatOptions options = {UMI_DOCUMENT_ENCODING_UNKNOWN, UMI_DOCUMENT_LINE_ENDING_CRLF, 0};
    UmiStatus wanted = UMI_STATUS_OK;
    int changed = 1, notify = 1;
    view.dirty = 1;
    view.cursor_offset = 2U;
    view.selection_length = 1U;
    if (strcmp(name, "lf") == 0)
    {
        source = "a\r\nb\r\n";
        expected = "a\nb\n";
        view.cursor_offset = 3U;
        options.line_ending = UMI_DOCUMENT_LINE_ENDING_LF;
    }
    if (strcmp(name, "encoding-only") == 0)
    {
        options.encoding = UMI_DOCUMENT_ENCODING_UTF8_BOM;
        options.line_ending = UMI_DOCUMENT_LINE_ENDING_NONE;
        expected = source;
    }
    if (strcmp(name, "unchanged") == 0)
    {
        options.line_ending = UMI_DOCUMENT_LINE_ENDING_LF;
        expected = source;
        changed = 0;
    }
    if (strcmp(name, "read-only") == 0)
    {
        view.read_only = 1;
        wanted = UMI_STATUS_PERMISSION_DENIED;
    }
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, source, strlen(source)) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
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
    if (strcmp(name, "parent-close") == 0)
    {
        gtk_window_destroy(GTK_WINDOW(umi_gtk4_adapter_native_window(result.adapter)));
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
    CHECK(UmiGtk4AdapterSetDocumentFormat(strcmp(name, "null-adapter") == 0 ? NULL : result.adapter,
                                          &options) == wanted);
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
            CHECK(umi_document_coordinator_active_snapshot(documents, &after) == UMI_STATUS_OK &&
                  after.encoding == before.encoding && after.line_ending == before.line_ending);
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
