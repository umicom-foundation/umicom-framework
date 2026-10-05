/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_workspace_action_pull_gtk4.c
 * PURPOSE: Check multi-draft action selection, diagnostic context and complete GTK edit review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workspace_edit_review.h"
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
static GtkWindow *Review(const char *tag)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i)
    {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (Find(GTK_WIDGET(window), tag) != NULL)
            return window;
        g_object_unref(window);
    }
    return NULL;
}

static int Wait(GtkWidget *window)
{
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (g_object_get_data(G_OBJECT(window), "umicom-completion-pending") != NULL &&
           g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return g_object_get_data(G_OBJECT(window), "umicom-completion-pending") == NULL;
}

/* A synchronous widget observer can edit inputs during result publication.
 * Such an edit must retire the proposal before a later click can apply it. */

static int TextEquals(UmiUiDocumentViewModel *views, const char *id, const char *expected)
{
    char *text = NULL;
    size_t bytes = 0U;
    UmiStatus status = UmiUiDocumentViewModelCopyText(views, id, &text, &bytes);
    int same = status == UMI_STATUS_OK && bytes == strlen(expected) && strcmp(text, expected) == 0;
    UmiUiDocumentViewModelFreeText(text);
    return same;
}
static int ProgramMain(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {"apply-all",       "undo",
                           "empty-context",   "readonly-dependency",
                           "readonly-target", "stale-dependency",
                           "stale-primary",   "changed-back",
                           "toggle",          "worker-unbind",
                           "parent-close",    "unknown",
                           "version-stale",   "invalid-range",
                           "other-language",  "single-option",
                           "private",         "choices",
                           "command",         "disabled",
                           "pull-toggle",     "pull-toggle-back",
                           "context-toggle",  "diagnostic-error",
                           "unchanged",       "related-report"};
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
    GtkWindow *dialog = NULL, *workspace = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    char *directory = g_get_current_dir();
    Completion result = {0};
    UmiDocumentWorkingCopySnapshot drafts[3];
    UmiUiDocumentViewSnapshot views_at[3];
    const char *names[] = {"main.c", "second.c", "third.c"};
    const char *texts[] = {"source main", "source second", "source third"};
    const char *uris[] = {"file:///workspace/main.c", "file:///workspace/second.c",
                          "file:///workspace/third.c"};
    application = gtk_application_new("org.umicom.workspace.actions.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("workspace.actions", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.workspace.actions.test", "Rename", workbench, &shell) ==
          UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    for (size_t i = 0U; i < 3U; ++i)
    {
        CHECK(umi_document_coordinator_new(documents, names[i], NULL, 0U) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(documents, &drafts[i]) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views, drafts[i].view_id, &views_at[i]) == UMI_STATUS_OK);
        strcpy(views_at[i].uri, uris[i]);
        strcpy(views_at[i].language_id, "c");
        views_at[i].dirty = 1;
        if ((i == 2U && strcmp(mode, "readonly-dependency") == 0) ||
            (i == 1U && strcmp(mode, "readonly-target") == 0))
            views_at[i].read_only = 1;
        CHECK(UmiUiDocumentViewModelUpsertText(views, &views_at[i], texts[i], strlen(texts[i])) ==
              UMI_STATUS_OK);
    }
    if (strcmp(mode, "other-language") == 0)
    {
        CHECK(umi_document_coordinator_new(documents, "other.py", NULL, 0U) == UMI_STATUS_OK);
        UmiDocumentWorkingCopySnapshot other;
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_document_coordinator_active_snapshot(documents, &other) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views, other.view_id, &view) == UMI_STATUS_OK);
        strcpy(view.language_id, "python");
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "do not send", 11U) == UMI_STATUS_OK);
    }
    CHECK(umi_ui_document_view_model_activate(views, drafts[0].view_id) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Complete, &result) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterReviewCodeActions(adapter, directory) == UMI_STATUS_OK);
    dialog = Review("document.completion.review");
    CHECK(dialog != NULL);
    GtkWidget *program = Find(GTK_WIDGET(dialog), "document.completion.program"),
              *arguments = Find(GTK_WIDGET(dialog), "document.completion.arguments");
    GtkWidget *request = Find(GTK_WIDGET(dialog), "document.completion.request"),
              *preview = Find(GTK_WIDGET(dialog), "document.completion.preview");
    GtkWidget *include = Find(GTK_WIDGET(dialog), "document.code-actions.open-drafts");
    CHECK(GTK_IS_EDITABLE(program) && GTK_IS_EDITABLE(arguments) && GTK_IS_BUTTON(preview) &&
          GTK_IS_BUTTON(request) && GTK_IS_CHECK_BUTTON(include));
    CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(include)));
    if (strcmp(mode, "single-option") == 0)
        goto unchanged;
    GtkWidget *diagnostic_context = Find(GTK_WIDGET(dialog), "document.code-actions.diagnostics");
    GtkWidget *pull_context = Find(GTK_WIDGET(dialog), "document.code-actions.pull");
    CHECK(GTK_IS_CHECK_BUTTON(diagnostic_context) && GTK_IS_CHECK_BUTTON(pull_context));
    CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(pull_context)));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(diagnostic_context), TRUE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(pull_context), TRUE);
    const char *peer = "actions-pull-valid";
    if (strcmp(mode, "apply-all") == 0 || strcmp(mode, "undo") == 0)
        peer = "actions-pull-all";
    if (strcmp(mode, "unknown") == 0)
        peer = "actions-pull-unknown";
    if (strcmp(mode, "version-stale") == 0)
        peer = "actions-pull-stale";
    if (strcmp(mode, "invalid-range") == 0)
        peer = "actions-pull-outside";
    if (strcmp(mode, "diagnostics") == 0)
        peer = "actions-pull-diagnostics";
    if (strcmp(mode, "choices") == 0)
        peer = "actions-pull-choices";
    if (strcmp(mode, "command") == 0)
        peer = "actions-pull-command";
    if (strcmp(mode, "disabled") == 0)
        peer = "actions-pull-disabled";
    if (strcmp(mode, "diagnostic-error") == 0)
        peer = "actions-pull-diagnostic-error";
    if (strcmp(mode, "unchanged") == 0)
        peer = "actions-pull-unchanged";
    if (strcmp(mode, "related-report") == 0)
        peer = "actions-pull-related-report";
    if (strcmp(mode, "empty-context") == 0)
        peer = "actions-pull-empty-context";
    gtk_editable_set_text(GTK_EDITABLE(program), argv[2]);
    gtk_editable_set_text(GTK_EDITABLE(arguments), peer);

    gtk_check_button_set_active(GTK_CHECK_BUTTON(include), TRUE);
    if (strcmp(mode, "diagnostics") == 0)
    {
        GtkWidget *diagnostics = Find(GTK_WIDGET(dialog), "document.code-actions.diagnostics");
        CHECK(GTK_IS_CHECK_BUTTON(diagnostics));
        gtk_check_button_set_active(GTK_CHECK_BUTTON(diagnostics), TRUE);
    }
    g_signal_emit_by_name(request, "clicked");
    if (strcmp(mode, "stale-dependency") == 0 || strcmp(mode, "stale-primary") == 0 ||
        strcmp(mode, "changed-back") == 0)
    {
        size_t index = strcmp(mode, "stale-primary") == 0 ? 0U : 2U;
        CHECK(UmiUiDocumentViewModelUpsertText(views, &views_at[index], "later", 5U) == UMI_STATUS_OK);
        if (strcmp(mode, "changed-back") == 0)
            CHECK(UmiUiDocumentViewModelUpsertText(views, &views_at[index], texts[index],
                                                   strlen(texts[index])) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "pull-toggle") == 0 || strcmp(mode, "pull-toggle-back") == 0)
    {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(pull_context), FALSE);
        if (strcmp(mode, "pull-toggle-back") == 0)
            gtk_check_button_set_active(GTK_CHECK_BUTTON(pull_context), TRUE);
    }
    if (strcmp(mode, "context-toggle") == 0)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(diagnostic_context), FALSE);
    if (strcmp(mode, "toggle") == 0)
    {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(include), FALSE);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(include), TRUE);
    }
    if (strcmp(mode, "worker-unbind") == 0)
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
    if (strcmp(mode, "parent-close") == 0)
        gtk_window_destroy(GTK_WINDOW(umi_gtk4_adapter_native_window(adapter)));
    CHECK(Wait(GTK_WIDGET(dialog)));
    GtkWidget *choice = Find(GTK_WIDGET(dialog), "document.completion.choice");
    CHECK(GTK_IS_DROP_DOWN(choice));
    if (strcmp(mode, "choices") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(preview));
        gtk_drop_down_set_selected(GTK_DROP_DOWN(choice), 1U);
        CHECK(gtk_widget_get_sensitive(preview));
    }
    if (strcmp(mode, "command") == 0 || strcmp(mode, "disabled") == 0)
        CHECK(!gtk_widget_get_sensitive(preview));
    /* Retained activation still cannot turn a command or disabled action into
     * an applied edit. The production catalogue enforces that boundary. */
    g_signal_emit_by_name(preview, "clicked");
    workspace = Review("document.workspace-edit.review");
    if (strcmp(mode, "pull-toggle") == 0 || strcmp(mode, "pull-toggle-back") == 0 ||
        strcmp(mode, "context-toggle") == 0 || strcmp(mode, "diagnostic-error") == 0 ||
        strcmp(mode, "unchanged") == 0 || strcmp(mode, "related-report") == 0)
    {
        CHECK(workspace == NULL && !gtk_widget_get_sensitive(preview));
        goto unchanged;
    }
    if (strcmp(mode, "readonly-target") == 0 || strcmp(mode, "stale-dependency") == 0 ||
        strcmp(mode, "stale-primary") == 0 || strcmp(mode, "changed-back") == 0 ||
        strcmp(mode, "toggle") == 0 || strcmp(mode, "worker-unbind") == 0 || strcmp(mode, "command") == 0 ||
        strcmp(mode, "disabled") == 0 || strcmp(mode, "parent-close") == 0 || strcmp(mode, "unknown") == 0 ||
        strcmp(mode, "version-stale") == 0 || strcmp(mode, "invalid-range") == 0)
    {
        CHECK(workspace == NULL);
        goto unchanged;
    }
    CHECK(workspace != NULL && UmiGtk4AdapterReplacementReviewBusy(adapter));
    if (strcmp(mode, "private") == 0)
    {
        CHECK(UmiGtk4RecordingIsPrivate(GTK_WIDGET(workspace)));
        goto unchanged;
    }
    GtkWidget *mark = Find(GTK_WIDGET(workspace), "document.workspace-edit.reviewed"),
              *next = Find(GTK_WIDGET(workspace), "document.workspace-edit.next");
    GtkWidget *approval = Find(GTK_WIDGET(workspace), "document.workspace-edit.approval"),
              *apply = Find(GTK_WIDGET(workspace), "document.workspace-edit.apply");
    CHECK(GTK_IS_BUTTON(mark) && GTK_IS_BUTTON(next) && GTK_IS_CHECK_BUTTON(approval) &&
          GTK_IS_BUTTON(apply));
    size_t changed = strcmp(mode, "apply-all") == 0 || strcmp(mode, "undo") == 0 ? 3U : 1U;
    for (size_t i = 0U; i < changed; ++i)
    {
        g_signal_emit_by_name(mark, "clicked");
        if (i + 1U < changed)
            g_signal_emit_by_name(next, "clicked");
    }
    CHECK(gtk_widget_get_sensitive(approval));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(approval), TRUE);
    g_signal_emit_by_name(apply, "clicked");
    CHECK(result.count == 1U && result.status == UMI_STATUS_OK);
    CHECK(TextEquals(views, drafts[1].view_id, "renamed second"));
    CHECK(TextEquals(views, drafts[0].view_id, changed == 3U ? "renamed main" : "source main"));
    CHECK(TextEquals(views, drafts[2].view_id, changed == 3U ? "renamed third" : "source third"));
    if (strcmp(mode, "undo") == 0)
        CHECK(UmiDocumentCoordinatorUndo(documents, drafts[1].document_id) == UMI_STATUS_OK &&
              TextEquals(views, drafts[1].view_id, "source second"));
    goto cleanup;
unchanged:
    for (size_t i = 0U; i < 3U; ++i)
    {
        const char *expected = (i == 0U && strcmp(mode, "stale-primary") == 0) ||
                                       (i == 2U && strcmp(mode, "stale-dependency") == 0)
                                   ? "later"
                                   : texts[i];
        CHECK(TextEquals(views, drafts[i].view_id, expected));
    }
    CHECK(result.count == 0U);
cleanup:
    if (adapter != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (workspace != NULL)
    {
        gtk_window_destroy(workspace);
        g_object_unref(workspace);
    }
    if (dialog != NULL)
    {
        gtk_window_destroy(dialog);
        (void)Wait(GTK_WIDGET(dialog));
        g_object_unref(dialog);
    }
    umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    g_free(directory);
    return failed;
}
#include "../native_process/utf8_entry.inc"
