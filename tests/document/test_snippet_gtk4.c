/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_snippet_gtk4.c
 * PURPOSE: Exercise local snippet composition, explicit approval, complete source changes and native lifetime boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/ui/gtk4/interaction_recording.h"
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
static void RetireOnModel(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    SetText(GTK_WIDGET(data), "changed template");
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {"open",
                           "private",
                           "expand",
                           "update",
                           "linked",
                           "selection",
                           "empty",
                           "multiline",
                           "unicode",
                           "final-stop",
                           "literal",
                           "preview",
                           "approval",
                           "apply",
                           "undo",
                           "stale",
                           "template-change",
                           "value-change",
                           "retained",
                           "native-close",
                           "unbind",
                           "parent-close",
                           "creation-unbind",
                           "completion-busy",
                           "cancel-command",
                           "choice",
                           "choices",
                           "invalid-value",
                           "oversized-template",
                           "model-retire",
                           "callback-unbind"};
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
    GtkWidget *retained = NULL;
    char *output = NULL;
    size_t output_bytes = 0U;
    gchar *shown = NULL;
    Completion result = {0};
    gulong observer = 0U;
    GtkWidget *choice = NULL;
    application = gtk_application_new("org.umicom.snippet.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("snippet.native", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.snippet.test", "Snippet", workbench, &shell) ==
          UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "main.c", NULL, 0U) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Complete, &result) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
    const char *original = strcmp(mode, "empty") == 0 ? "" : "left RIGHT";
    view.cursor_offset = original[0] == '\0' ? 0U : 5U;
    view.selection_length = strcmp(mode, "selection") == 0 ? 5U : 0U;
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, original, strlen(original)) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_sync_active(documents) == UMI_STATUS_OK);
    if (strcmp(mode, "creation-unbind") == 0)
    {
        result.detach = adapter;
        gulong hook = g_signal_connect(gtk_window_get_toplevels(), "items-changed",
                                       G_CALLBACK(UnbindOnCreate), &result);
        UmiStatus status = UmiGtk4AdapterReviewSnippet(adapter);
        g_signal_handler_disconnect(gtk_window_get_toplevels(), hook);
        CHECK(status == UMI_STATUS_CANCELLED && !UmiGtk4AdapterCompletionReviewBusy(adapter));
        CHECK(Review() == NULL);
        goto inspect;
    }
    CHECK(UmiGtk4AdapterReviewSnippet(adapter) == UMI_STATUS_OK);
    dialog = Review();
    CHECK(dialog != NULL);
    GtkWidget *body = Find(GTK_WIDGET(dialog), "document.snippet.template"),
              *value = Find(GTK_WIDGET(dialog), "document.snippet.value");
    GtkWidget *expand = Find(GTK_WIDGET(dialog), "document.snippet.expand"),
              *update = Find(GTK_WIDGET(dialog), "document.snippet.update");
    GtkWidget *expanded = Find(GTK_WIDGET(dialog), "document.snippet.expanded"),
              *preview = Find(GTK_WIDGET(dialog), "document.snippet.preview");
    GtkWidget *approve = Find(GTK_WIDGET(dialog), "document.snippet.approve"),
              *apply = Find(GTK_WIDGET(dialog), "document.snippet.apply");
    GtkWidget *cancel = Find(GTK_WIDGET(dialog), "document.snippet.cancel");
    choice = Find(GTK_WIDGET(dialog), "document.snippet.choice");
    CHECK(body && value && expand && update && expanded && preview && approve && apply && cancel && choice);
    CHECK(!gtk_widget_get_sensitive(apply) && gtk_window_get_default_widget(dialog) == cancel);
    CHECK(UmiGtk4AdapterCompletionReviewBusy(adapter));
    retained = g_object_ref(apply);
    if (strcmp(mode, "open") == 0)
        goto inspect;
    if (strcmp(mode, "private") == 0)
    {
        CHECK(UmiGtk4RecordingIsPrivate(GTK_WIDGET(dialog)));
        goto inspect;
    }
    if (strcmp(mode, "completion-busy") == 0)
    {
        CHECK(UmiGtk4AdapterReviewSnippet(adapter) == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterReviewCompletion(adapter, NULL) == UMI_STATUS_BUSY);
        goto inspect;
    }
    if (strcmp(mode, "cancel-command") == 0)
    {
        CHECK(UmiGtk4AdapterCancelCompletionReview(adapter) == UMI_STATUS_OK &&
              !UmiGtk4AdapterCompletionReviewBusy(adapter));
        CHECK(result.count == 1U && result.status == UMI_STATUS_CANCELLED);
        goto inspect;
    }
    const char *body_text = strcmp(mode, "final-stop") == 0 ? "a$0b"
                            : strcmp(mode, "literal") == 0  ? "literal"
                            : strcmp(mode, "choice") == 0   ? "${1:first}-${2:second}-$2$0"
                            : strcmp(mode, "choices") == 0  ? "${1|one,two|}-$1$0"
                                                            : "${1:name}-$1$0";
    SetText(body, body_text);
    if (strcmp(mode, "oversized-template") == 0)
    {
        gchar *large = g_malloc0(8193U);
        memset(large, 'x', 8192U);
        SetText(body, large);
        g_free(large);
        g_signal_emit_by_name(expand, "clicked");
        g_signal_emit_by_name(preview, "clicked");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
        CHECK(!gtk_widget_get_sensitive(apply));
        goto inspect;
    }
    if (strcmp(mode, "model-retire") == 0)
        observer = g_signal_connect(choice, "notify::model", G_CALLBACK(RetireOnModel), body);
    g_signal_emit_by_name(expand, "clicked");
    if (strcmp(mode, "model-retire") == 0)
    {
        g_signal_emit_by_name(preview, "clicked");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
        g_signal_emit_by_name(apply, "clicked");
        CHECK(!gtk_widget_get_sensitive(apply));
        goto inspect;
    }
    const char *expected_expanded = strcmp(mode, "final-stop") == 0 ? "ab"
                                    : strcmp(mode, "literal") == 0  ? "literal"
                                    : strcmp(mode, "choice") == 0   ? "first-second-second"
                                    : strcmp(mode, "choices") == 0  ? "one-one"
                                                                    : "name-name";
    shown = Text(expanded);
    CHECK(strcmp(shown, expected_expanded) == 0);
    g_clear_pointer(&shown, g_free);
    if (strcmp(mode, "expand") == 0)
        goto inspect;
    if (strcmp(mode, "choice") == 0)
    {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(choice), 1U);
        shown = Text(value);
        CHECK(strcmp(shown, "second") == 0);
        g_clear_pointer(&shown, g_free);
    }
    if (strcmp(mode, "update") == 0 || strcmp(mode, "linked") == 0 || strcmp(mode, "unicode") == 0 ||
        strcmp(mode, "multiline") == 0 || strcmp(mode, "choice") == 0 || strcmp(mode, "choices") == 0)
    {
        const char *replacement = strcmp(mode, "unicode") == 0     ? "caf\xc3\xa9"
                                  : strcmp(mode, "multiline") == 0 ? "a\nb"
                                                                   : "value";
        SetText(value, replacement);
        g_signal_emit_by_name(update, "clicked");
        expected_expanded = strcmp(mode, "unicode") == 0     ? "caf\xc3\xa9-caf\xc3\xa9"
                            : strcmp(mode, "multiline") == 0 ? "a\nb-a\nb"
                            : strcmp(mode, "choice") == 0    ? "first-value-value"
                                                             : "value-value";
        shown = Text(expanded);
        CHECK(strcmp(shown, expected_expanded) == 0);
        g_clear_pointer(&shown, g_free);
    }
    if (strcmp(mode, "invalid-value") == 0)
    {
        gchar large[513];
        memset(large, 'x', 512U);
        large[512] = '\0';
        SetText(value, large);
        g_signal_emit_by_name(update, "clicked");
        g_signal_emit_by_name(preview, "clicked");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
        CHECK(!gtk_widget_get_sensitive(apply));
        g_signal_emit_by_name(apply, "clicked");
        goto inspect;
    }
    g_signal_emit_by_name(preview, "clicked");
    CHECK(gtk_widget_get_first_child(Find(GTK_WIDGET(dialog), "document.snippet.comparison")) != NULL);
    if (strcmp(mode, "preview") == 0)
        goto inspect;
    if (strcmp(mode, "approval") == 0)
    {
        g_signal_emit_by_name(apply, "clicked");
        goto inspect;
    }
    gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
    CHECK(gtk_widget_get_sensitive(apply));
    if (strcmp(mode, "template-change") == 0 || strcmp(mode, "value-change") == 0)
    {
        SetText(strcmp(mode, "template-change") == 0 ? body : value, "different");
        CHECK(!gtk_widget_get_sensitive(apply));
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
        g_signal_emit_by_name(apply, "clicked");
        goto inspect;
    }
    if (strcmp(mode, "retained") == 0 || strcmp(mode, "native-close") == 0 || strcmp(mode, "unbind") == 0 ||
        strcmp(mode, "parent-close") == 0)
    {
        if (strcmp(mode, "retained") == 0)
            g_signal_emit_by_name(cancel, "clicked");
        else if (strcmp(mode, "native-close") == 0)
            gtk_window_destroy(dialog);
        else if (strcmp(mode, "unbind") == 0)
            CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        else
            gtk_window_destroy(GTK_WINDOW(umi_gtk4_adapter_native_window(adapter)));
        g_signal_emit_by_name(retained, "clicked");
        g_signal_emit_by_name(update, "clicked");
        CHECK(!UmiGtk4AdapterCompletionReviewBusy(adapter));
        goto inspect;
    }
    if (strcmp(mode, "stale") == 0)
    {
        view.cursor_offset = 0U;
        view.selection_length = 0U;
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "changed", 7U) == UMI_STATUS_OK);
        original = "changed";
        g_signal_emit_by_name(apply, "clicked");
        CHECK(!gtk_widget_get_sensitive(apply));
        goto inspect;
    }
    if (strcmp(mode, "callback-unbind") == 0)
        result.detach = adapter;
    g_signal_emit_by_name(apply, "clicked");
    CHECK(result.count == 1U && result.status == UMI_STATUS_OK);
    CHECK(!UmiGtk4AdapterCompletionReviewBusy(adapter));
    char expected[128];
    (void)snprintf(expected, sizeof(expected), "%.*s%s%s", (int)view.cursor_offset, original,
                   expected_expanded, original + view.cursor_offset + view.selection_length);
    CHECK(UmiUiDocumentViewModelCopyText(views, active.view_id, &output, &output_bytes) == UMI_STATUS_OK);
    CHECK(output_bytes == strlen(expected) && strcmp(output, expected) == 0);
    UmiUiDocumentViewModelFreeText(output);
    output = NULL;
    if (strcmp(mode, "undo") == 0)
    {
        CHECK(UmiDocumentCoordinatorUndo(documents, active.document_id) == UMI_STATUS_OK);
        goto inspect;
    }
    goto cleanup;
inspect:
    CHECK(UmiUiDocumentViewModelCopyText(views, active.view_id, &output, &output_bytes) == UMI_STATUS_OK);
    CHECK(output_bytes == strlen(original) && strcmp(output, original) == 0);
cleanup:
    if (observer != 0U && choice != NULL)
        g_signal_handler_disconnect(choice, observer);
    g_free(shown);
    UmiUiDocumentViewModelFreeText(output);
    if (adapter != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (dialog != NULL)
        gtk_window_destroy(dialog);
    g_clear_object(&dialog);
    g_clear_object(&retained);
    umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    return failed;
}
