/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_proposal_gtk4.c
 * PURPOSE: Exercise the real proposal window against in-memory documents, including native reentrancy and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
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
/* The rendered-only search missed controls in collapsed review panels. The Framework logical-tree helper replaces it; retain the former traversal for review. */
#if 0
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
#endif
/* Inspect logical ownership as well as rendered children. Finding a control
 * does not grant permission to edit it or make a collapsed panel visible. */
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    return umi_gtk4_automation_find_tagged_widget(root, id);
}
static GtkWindow *Review(void)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i)
    {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (Find(GTK_WIDGET(window), "document.proposal.review") != NULL)
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
typedef struct DuringPreview
{
    UmiGtk4Adapter *adapter;
    GtkTextBuffer *input;
    int edit, called;
} DuringPreview;
static gboolean MutateDuringPreview(GSignalInvocationHint *hint, guint count, const GValue *values,
                                    gpointer data)
{
    (void)hint;
    DuringPreview *change = data;
    if (!change->called && count > 0U && g_value_get_object(&values[0]) != G_OBJECT(change->input))
    {
        change->called = 1;
        if (change->edit)
            gtk_text_buffer_set_text(change->input, "unreviewed", -1);
        else
            (void)UmiGtk4AdapterBindDocumentEditing(change->adapter, NULL, NULL, NULL);
    }
    return TRUE;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"apply",
                           "undo",
                           "no-preview",
                           "no-approval",
                           "edit-after-preview",
                           "stale",
                           "cancel",
                           "retained",
                           "unbind",
                           "parent-close",
                           "creation-unbind",
                           "preview-unbind",
                           "preview-edit",
                           "completion-unbind",
                           "busy",
                           "private",
                           "delete"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    GtkApplication *application = NULL;
    GtkWindow *dialog = NULL;
    GtkWidget *apply = NULL, *input = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    Completion result = {0};
    UmiDocumentWorkingCopySnapshot active;
    UmiUiDocumentViewSnapshot view;
    char *text = NULL;
    size_t length = 0U;
    application = gtk_application_new("org.umicom.proposal.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.proposal.native", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.proposal.test", "Proposal", workbench, &shell) ==
          UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "review.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    view.cursor_offset = 7U;
    view.selection_length = 3U;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "before old after", 16U) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Complete, &result) == UMI_STATUS_OK);
    /* Native preparation may synchronize its initial view. Capture the intended
     * source selection after that setup, as the user's selection action does. */
    CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
    view.cursor_offset = 7U;
    view.selection_length = 3U;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    if (strcmp(name, "creation-unbind") == 0)
    {
        result.detach = adapter;
        gulong observer = g_signal_connect(gtk_window_get_toplevels(), "items-changed",
                                           G_CALLBACK(UnbindOnCreate), &result);
        UmiStatus status = UmiGtk4AdapterReviewSelectedCode(adapter);
        g_signal_handler_disconnect(gtk_window_get_toplevels(), observer);
        CHECK(status == UMI_STATUS_CANCELLED && !UmiGtk4AdapterDocumentProposalBusy(adapter) &&
              result.count == 0U);
        dialog = Review();
        CHECK(dialog == NULL);
        goto inspect;
    }
    CHECK(UmiGtk4AdapterReviewSelectedCode(adapter) == UMI_STATUS_OK);
    dialog = Review();
    CHECK(dialog != NULL && UmiGtk4AdapterDocumentProposalBusy(adapter));
    GtkWidget *found_apply = Find(GTK_WIDGET(dialog), "document.proposal.apply");
    GtkWidget *found_input = Find(GTK_WIDGET(dialog), "document.proposal.input");
    CHECK(found_apply != NULL && found_input != NULL);
    apply = g_object_ref(found_apply);
    input = g_object_ref(found_input);
    GtkWidget *preview = Find(GTK_WIDGET(dialog), "document.proposal.preview");
    GtkWidget *approve = Find(GTK_WIDGET(dialog), "document.proposal.approve");
    GtkWidget *cancel = Find(GTK_WIDGET(dialog), "document.proposal.cancel");
    CHECK(preview != NULL && approve != NULL && cancel != NULL &&
          gtk_window_get_default_widget(dialog) == cancel);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(input));
    gtk_text_buffer_set_text(buffer, strcmp(name, "delete") == 0 ? "" : "new", -1);
    if (strcmp(name, "busy") == 0)
    {
        CHECK(UmiGtk4AdapterReviewSelectedCode(adapter) == UMI_STATUS_BUSY);
        goto inspect;
    }
    if (strcmp(name, "cancel") == 0)
    {
        g_signal_emit_by_name(cancel, "clicked");
        CHECK(!UmiGtk4AdapterDocumentProposalBusy(adapter) && result.count == 1U &&
              result.status == UMI_STATUS_CANCELLED);
        goto inspect;
    }
    if (strcmp(name, "unbind") == 0 || strcmp(name, "retained") == 0 || strcmp(name, "parent-close") == 0)
    {
        if (strcmp(name, "unbind") == 0)
            CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        else if (strcmp(name, "retained") == 0)
            gtk_window_destroy(dialog);
        else
        {
            GtkWidget *parent = g_object_ref(umi_gtk4_adapter_native_window(adapter));
            gtk_window_destroy(GTK_WINDOW(parent));
            g_object_unref(parent);
        }
        CHECK(!UmiGtk4AdapterDocumentProposalBusy(adapter) && gtk_text_buffer_get_char_count(buffer) == 0);
        unsigned before = result.count;
        g_signal_emit_by_name(apply, "clicked");
        CHECK(result.count == before);
        goto inspect;
    }
    if (strcmp(name, "preview-unbind") == 0 || strcmp(name, "preview-edit") == 0)
    {
        DuringPreview change = {adapter, buffer, strcmp(name, "preview-edit") == 0, 0};
        guint signal = g_signal_lookup("changed", GTK_TYPE_TEXT_BUFFER);
        gulong hook = g_signal_add_emission_hook(signal, 0, MutateDuringPreview, &change, NULL);
        CHECK(hook != 0UL);
        g_signal_emit_by_name(preview, "clicked");
        g_signal_remove_emission_hook(signal, hook);
        CHECK(change.called && !gtk_widget_get_sensitive(apply));
        g_signal_emit_by_name(apply, "clicked");
        CHECK(result.count == 0U);
        goto inspect;
    }
    if (strcmp(name, "no-preview") != 0)
        g_signal_emit_by_name(preview, "clicked");
    if (strcmp(name, "private") == 0)
    {
        GtkWidget *left = Find(GTK_WIDGET(dialog), "umicom.comparison.left");
        CHECK(left != NULL);
        CHECK(UmiGtk4RecordingIsPrivate(input) && UmiGtk4RecordingIsPrivate(left));
        goto inspect;
    }
    if (strcmp(name, "no-approval") != 0)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
    if (strcmp(name, "edit-after-preview") == 0)
    {
        gtk_text_buffer_set_text(buffer, "unreviewed", -1);
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(approve)) && !gtk_widget_get_sensitive(apply));
    }
    if (strcmp(name, "stale") == 0)
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "later source text", 17U) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
    if (strcmp(name, "completion-unbind") == 0)
        result.detach = adapter;
    g_signal_emit_by_name(apply, "clicked");
    if (strcmp(name, "apply") == 0 || strcmp(name, "undo") == 0 || strcmp(name, "completion-unbind") == 0 ||
        strcmp(name, "delete") == 0)
    {
        CHECK(result.count == 1U && result.status == UMI_STATUS_OK &&
              !UmiGtk4AdapterDocumentProposalBusy(adapter));
        g_signal_emit_by_name(apply, "clicked");
        CHECK(result.count == 1U);
        if (strcmp(name, "undo") == 0)
            CHECK(UmiDocumentCoordinatorUndo(documents, active.document_id) == UMI_STATUS_OK);
    }
    else
        CHECK(result.count == 0U);
inspect:
    CHECK(UmiUiDocumentViewModelCopyText(views, active.view_id, &text, &length) == UMI_STATUS_OK);
    const char *expected = strcmp(name, "apply") == 0 || strcmp(name, "completion-unbind") == 0
                               ? "before new after"
                           : strcmp(name, "delete") == 0 ? "before  after"
                           : strcmp(name, "stale") == 0  ? "later source text"
                                                         : "before old after";
    CHECK(length == strlen(expected) && strcmp(text, expected) == 0);
cleanup:
    UmiUiDocumentViewModelFreeText(text);
    if (adapter != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (dialog != NULL)
        gtk_window_destroy(dialog);
    g_clear_object(&dialog);
    g_clear_object(&apply);
    g_clear_object(&input);
    umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    return failed;
}
