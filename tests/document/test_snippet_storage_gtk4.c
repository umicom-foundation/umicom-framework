/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_snippet_storage_gtk4.c
 * PURPOSE: Check named template worker publication, explicit persistence and retirement without editing real user projects.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/developer_project/snippet_storage.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/local_replace.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
typedef struct Completion
{
    unsigned count;
    UmiStatus status;
    UmiGtk4Adapter *detach;
} Completion;
static void Complete(void *data, UmiStatus status)
{
    Completion *completion = data;
    ++completion->count;
    completion->status = status;
    if (completion->detach != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(completion->detach, NULL, NULL, NULL);
}
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *actual = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (actual != NULL && strcmp(actual, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
static GtkWindow *Review(void)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i)
    {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (Find(GTK_WIDGET(window), "document.snippet.review") != NULL)
            return window;
        g_object_unref(window);
    }
    return NULL;
}
static void UnbindOnCreate(GListModel *model, guint position, guint removed, guint added, gpointer data)
{
    (void)model;
    (void)position;
    (void)removed;
    Completion *result = data;
    if (added != 0U && result->detach != NULL)
    {
        UmiGtk4Adapter *adapter = result->detach;
        result->detach = NULL;
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    }
}

static void SetText(GtkWidget *view, const char *text)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), text, -1);
}
static gchar *Text(GtkWidget *view)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter a, b;
    gtk_text_buffer_get_bounds(buffer, &a, &b);
    return gtk_text_buffer_get_text(buffer, &a, &b, FALSE);
}

static int Wait(GtkWidget *window)
{
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (g_object_get_data(G_OBJECT(window), "umicom-snippet-storage-pending") != NULL &&
           g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return g_object_get_data(G_OBJECT(window), "umicom-snippet-storage-pending") == NULL;
}
typedef struct Retire
{
    GtkWidget *field;
    int fired;
} Retire;
static void RetireLoad(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer;
    Retire *retire = data;
    if (!retire->fired)
    {
        retire->fired = 1;
        SetText(retire->field, "observer edit");
    }
}
static void RetireStart(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    Retire *retire = data;
    if (!retire->fired)
    {
        retire->fired = 1;
        gtk_editable_set_text(GTK_EDITABLE(retire->field), "new name");
    }
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {"open",
                           "creation-unbind",
                           "save",
                           "load",
                           "load-apply",
                           "load-expire-prepared",
                           "save-before-expand",
                           "overwrite",
                           "unicode",
                           "name-change",
                           "body-change",
                           "value-change",
                           "save-stale",
                           "worker-unbind",
                           "worker-close",
                           "retained",
                           "missing",
                           "malformed",
                           "wrong-language",
                           "wrong-id",
                           "blank-name",
                           "oversized-name",
                           "load-retire",
                           "busy",
                           "save-start-retire"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    GtkApplication *application = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    GtkWindow *dialog = NULL;
    gchar *directory = NULL, *shown = NULL;
    char *output = NULL;
    size_t output_bytes = 0U;
    UmiEditorSnippetTemplate *snippet = NULL, *loaded = NULL;
    GtkWidget *retained = NULL;
    gulong observer = 0U;
    GObject *observed = NULL;
    Completion completion = {0};
    Retire retire = {0};
    char path[UMI_PATH_CAPACITY], parent[UMI_PATH_CAPACITY];
    directory = g_dir_make_tmp("umicom-named-snippet-XXXXXX", NULL);
    CHECK(directory != NULL);
    application = gtk_application_new("org.umicom.snippet.storage.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("snippet.storage", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.snippet.storage.test", "Snippet storage", workbench,
                                          &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "main.c", NULL, 0U) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
    view.cursor_offset = 0U;
    view.selection_length = 0U;
    view.dirty = 1;
    strcpy(view.language_id, "c");
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Complete, &completion) == UMI_STATUS_OK);
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "source", 6U) == UMI_STATUS_OK);
    if (strcmp(mode, "creation-unbind") == 0)
    {
        completion.detach = adapter;
        gulong hook = g_signal_connect(gtk_window_get_toplevels(), "items-changed",
                                       G_CALLBACK(UnbindOnCreate), &completion);
        UmiStatus status = UmiGtk4AdapterReviewSnippetWithSettings(adapter, "SnippetTest", directory);
        g_signal_handler_disconnect(gtk_window_get_toplevels(), hook);
        CHECK(status == UMI_STATUS_CANCELLED && !UmiGtk4AdapterCompletionReviewBusy(adapter));
        CHECK(Review() == NULL);
        goto inspect;
    }
    CHECK(UmiGtk4AdapterReviewSnippetWithSettings(adapter, "SnippetTest", directory) == UMI_STATUS_OK);
    dialog = Review();
    CHECK(dialog != NULL);
    GtkWidget *body = Find(GTK_WIDGET(dialog), "document.snippet.template"),
              *name = Find(GTK_WIDGET(dialog), "document.snippet.name");
    GtkWidget *save = Find(GTK_WIDGET(dialog), "document.snippet.save"),
              *load = Find(GTK_WIDGET(dialog), "document.snippet.load");
    GtkWidget *expand = Find(GTK_WIDGET(dialog), "document.snippet.expand"),
              *preview = Find(GTK_WIDGET(dialog), "document.snippet.preview");
    GtkWidget *approve = Find(GTK_WIDGET(dialog), "document.snippet.approve"),
              *apply = Find(GTK_WIDGET(dialog), "document.snippet.apply");
    CHECK(body && name && save && load && expand && preview && approve && apply);
    retained = g_object_ref(load);
    const char *id = strcmp(mode, "unicode") == 0 ? "caf\xc3\xa9" : "loop";
    gtk_editable_set_text(GTK_EDITABLE(name), id);
    SetText(body, "draft template");
    CHECK(UmiSnippetTemplatePath("SnippetTest", directory, "c", id, path, sizeof(path)) == UMI_STATUS_OK);
    CHECK(!umi_fs_is_file(path));
    if (strcmp(mode, "open") == 0)
        goto inspect;
    if (strcmp(mode, "blank-name") == 0 || strcmp(mode, "oversized-name") == 0)
    {
        char large[129];
        memset(large, 'x', 128U);
        large[128] = '\0';
        gtk_editable_set_text(GTK_EDITABLE(name), strcmp(mode, "blank-name") == 0 ? "" : large);
        g_signal_emit_by_name(save, "clicked");
        CHECK(g_object_get_data(G_OBJECT(dialog), "umicom-snippet-storage-pending") == NULL);
        CHECK(gtk_widget_get_sensitive(save) && gtk_widget_get_sensitive(load));
        CHECK(!umi_fs_is_file(path));
        goto inspect;
    }
    if (strcmp(mode, "save-start-retire") == 0)
    {
        retire.field = name;
        observed = G_OBJECT(save);
        observer = g_signal_connect(save, "notify::sensitive", G_CALLBACK(RetireStart), &retire);
        g_signal_emit_by_name(save, "clicked");
        CHECK(retire.fired && g_object_get_data(G_OBJECT(dialog), "umicom-snippet-storage-pending") == NULL);
        CHECK(gtk_widget_get_sensitive(save) && gtk_widget_get_sensitive(load) && !umi_fs_is_file(path));
        goto inspect;
    }
    snippet = g_new0(UmiEditorSnippetTemplate, 1);
    loaded = g_new0(UmiEditorSnippetTemplate, 1);
    snippet->struct_size = (uint32_t)sizeof(*snippet);
    snippet->api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
    strcpy(snippet->id, id);
    strcpy(snippet->language_id, "c");
    strcpy(snippet->name, id);
    strcpy(snippet->body, "${1:saved}-$1$0");
    int saving = strcmp(mode, "save") == 0 || strcmp(mode, "save-before-expand") == 0 ||
                 strcmp(mode, "overwrite") == 0 || strcmp(mode, "save-stale") == 0 ||
                 strcmp(mode, "unicode") == 0;
    if (strcmp(mode, "missing") != 0 && (!saving || strcmp(mode, "overwrite") == 0))
    {
        CHECK(umi_path_parent(path, parent, sizeof(parent)) == UMI_STATUS_OK &&
              umi_fs_make_directories(parent) == UMI_STATUS_OK);
        if (strcmp(mode, "wrong-language") == 0)
            strcpy(snippet->language_id, "C");
        if (strcmp(mode, "wrong-id") == 0)
            strcpy(snippet->id, "different");
        CHECK(UmiSnippetTemplateSave(path, snippet) == UMI_STATUS_OK);
        if (strcmp(mode, "malformed") == 0)
            CHECK(UmiLocalFileReplace(path, "{", 1U) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "load-retire") == 0)
    {
        retire.field = body;
        observed = G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(body)));
        observer = g_signal_connect(observed, "changed", G_CALLBACK(RetireLoad), &retire);
    }
    if (strcmp(mode, "load-expire-prepared") == 0)
    {
        SetText(body, snippet->body);
        g_signal_emit_by_name(expand, "clicked");
        g_signal_emit_by_name(preview, "clicked");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
        CHECK(gtk_widget_get_sensitive(apply));
    }
    g_signal_emit_by_name(saving ? save : load, "clicked");
    CHECK(g_object_get_data(G_OBJECT(dialog), "umicom-snippet-storage-pending") != NULL);
    if (strcmp(mode, "busy") == 0)
    {
        gpointer pending = g_object_get_data(G_OBJECT(dialog), "umicom-snippet-storage-pending");
        g_signal_emit_by_name(save, "clicked");
        g_signal_emit_by_name(expand, "clicked");
        CHECK(g_object_get_data(G_OBJECT(dialog), "umicom-snippet-storage-pending") == pending);
    }
    if (strcmp(mode, "name-change") == 0)
        gtk_editable_set_text(GTK_EDITABLE(name), "other");
    if (strcmp(mode, "body-change") == 0 || strcmp(mode, "save-stale") == 0)
        SetText(body, "newer draft");
    if (strcmp(mode, "value-change") == 0)
        SetText(Find(GTK_WIDGET(dialog), "document.snippet.value"), "new value");
    if (strcmp(mode, "worker-unbind") == 0)
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
    if (strcmp(mode, "worker-close") == 0 || strcmp(mode, "retained") == 0)
        gtk_window_destroy(dialog);
    CHECK(Wait(GTK_WIDGET(dialog)));
    if (strcmp(mode, "retained") == 0)
        g_signal_emit_by_name(retained, "clicked");
    shown = Text(body);
    if (saving)
    {
        CHECK(UmiSnippetTemplateLoad(path, "c", id, loaded) == UMI_STATUS_OK &&
              strcmp(loaded->body, "draft template") == 0);
        CHECK(strcmp(shown, strcmp(mode, "save-stale") == 0 ? "newer draft" : "draft template") == 0);
    }
    else if (strcmp(mode, "body-change") == 0)
        CHECK(strcmp(shown, "newer draft") == 0);
    else if (strcmp(mode, "load-retire") == 0)
    {
        CHECK(retire.fired);
        CHECK(!gtk_widget_get_sensitive(apply));
    }
    else if (strcmp(mode, "name-change") == 0 || strcmp(mode, "value-change") == 0 ||
             strcmp(mode, "worker-unbind") == 0 || strcmp(mode, "worker-close") == 0 ||
             strcmp(mode, "retained") == 0 || strcmp(mode, "missing") == 0 ||
             strcmp(mode, "malformed") == 0 || strcmp(mode, "wrong-language") == 0 ||
             strcmp(mode, "wrong-id") == 0)
        CHECK(strcmp(shown, "draft template") == 0);
    else
    {
        CHECK(strcmp(shown, "${1:saved}-$1$0") == 0);
        g_signal_emit_by_name(preview, "clicked");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
        CHECK(!gtk_widget_get_sensitive(apply));
        if (strcmp(mode, "load-apply") == 0)
        {
            g_signal_emit_by_name(expand, "clicked");
            g_signal_emit_by_name(preview, "clicked");
            gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
            CHECK(gtk_widget_get_sensitive(apply));
            g_signal_emit_by_name(apply, "clicked");
        }
    }
inspect:
    CHECK(UmiUiDocumentViewModelCopyText(views, active.view_id, &output, &output_bytes) == UMI_STATUS_OK);
    const char *expected = strcmp(mode, "load-apply") == 0 ? "saved-savedsource" : "source";
    CHECK(output_bytes == strlen(expected) && strcmp(output, expected) == 0);
cleanup:
    if (observer != 0U)
        g_signal_handler_disconnect(observed, observer);
    if (adapter != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (dialog != NULL)
    {
        gtk_window_destroy(dialog);
        (void)Wait(GTK_WIDGET(dialog));
    }
    g_clear_object(&dialog);
    g_clear_object(&retained);
    g_free(shown);
    g_free(snippet);
    g_free(loaded);
    g_free(directory);
    UmiUiDocumentViewModelFreeText(output);
    umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    return failed;
}
