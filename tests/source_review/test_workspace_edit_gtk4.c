/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/source_review/test_workspace_edit_gtk4.c
 * PURPOSE: Exercise complete language edit comparisons, annotation gates and native lifetime boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workspace_edit_review.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
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
    Completion *result = data;
    ++result->count;
    result->status = status;
    if (result->detach != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(result->detach, NULL, NULL, NULL);
}

/* Inspect semantic controls, never GTK implementation child positions. */
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    if (root == NULL)
        return NULL;
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
    for (guint index = 0U; index < g_list_model_get_n_items(windows); ++index)
    {
        GtkWindow *window = g_list_model_get_item(windows, index);
        if (Find(GTK_WIDGET(window), "document.workspace-edit.review") != NULL)
            return window;
        g_object_unref(window);
    }
    return NULL;
}
/* A toplevel observer can replace a binding during native construction. */
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

static void Pump(void)
{
    /* Bound fixture work even if an unrelated source keeps the main loop busy. */
    for (unsigned iteration = 0U; iteration < 100U && g_main_context_pending(NULL); ++iteration)
        (void)g_main_context_iteration(NULL, FALSE);
}

static int TextEquals(UmiUiDocumentViewModel *views, const char *id, const char *expected)
{
    char *text = NULL;
    size_t bytes = 0U;
    UmiStatus status = UmiUiDocumentViewModelCopyText(views, id, &text, &bytes);
    int same = status == UMI_STATUS_OK && bytes == strlen(expected) && strcmp(text, expected) == 0;
    UmiUiDocumentViewModelFreeText(text);
    return same;
}

/* Construct real captured-source proposals, not an alternate test model. */
/* Annotated edits use the protocol documentChanges form; retain the initial fixture payload for review because changes maps cannot carry confirmation annotations.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus Model(UmiDocumentCoordinator *documents,UmiUiDocumentViewModel *views,
    const UmiDocumentWorkingCopySnapshot *first,const UmiDocumentWorkingCopySnapshot *second,
    const char *mode,UmiSourceWorkspaceEditReview **out_model)
{
    UmiUiDocumentViewSnapshot a={0},b={0};
    UmiStatus status=umi_ui_document_view_model_find(views,first->view_id,&a);
    if(status==UMI_STATUS_OK) status=umi_ui_document_view_model_find(views,second->view_id,&b);
    UmiDocumentId ids[]={first->document_id,second->document_id};
    UmiDocumentSourceWorkspace *sources=NULL;UmiLanguageWorkspaceEditCatalogue *catalogue=NULL;
    if(status==UMI_STATUS_OK) status=UmiDocumentSourceWorkspaceCreate(documents,ids,2U,&sources);
    char json[4096];int annotation=strncmp(mode,"annotation",10U)==0;
    const char *marker=annotation?",\"annotationId\":\"reason\"":"";
    int length=snprintf(json,sizeof(json),"{\"changes\":{\"%s\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":4}},\"newText\":\"saved\"%s},{\"range\":{\"start\":{\"line\":0,\"character\":5},\"end\":{\"line\":0,\"character\":9}},\"newText\":\"saved\"}],\"%s\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":4}},\"newText\":\"saved\"}]}%s}",a.uri,marker,b.uri,annotation?",\"changeAnnotations\":{\"reason\":{\"label\":\"<b>Provider reason</b>\",\"description\":\"A literal explanation\",\"needsConfirmation\":true}}":"");
    if(status==UMI_STATUS_OK && (length<0 || (size_t)length>=sizeof(json))) status=UMI_STATUS_CAPACITY_EXCEEDED;
    if(strcmp(mode,"unchanged")==0) (void)snprintf(json,sizeof(json),"{\"changes\":{\"%s\":[],\"%s\":[]}}",a.uri,b.uri);
    if(status==UMI_STATUS_OK) status=UmiLanguageWorkspaceEditCatalogueCreate(json,strlen(json),NULL,&catalogue);
    if(status==UMI_STATUS_OK) status=UmiSourceWorkspaceEditReviewCreate(documents,&sources,&catalogue,NULL,NULL,out_model);
    UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);UmiDocumentSourceWorkspaceDestroy(sources);return status;
}
#endif
static UmiStatus Model(UmiDocumentCoordinator *documents, UmiUiDocumentViewModel *views,
                       const UmiDocumentWorkingCopySnapshot *first,
                       const UmiDocumentWorkingCopySnapshot *second, const char *mode,
                       UmiSourceWorkspaceEditReview **out_model)
{
    UmiUiDocumentViewSnapshot a = {0}, b = {0};
    UmiStatus status = umi_ui_document_view_model_find(views, first->view_id, &a);
    if (status == UMI_STATUS_OK)
        status = umi_ui_document_view_model_find(views, second->view_id, &b);
    UmiDocumentId ids[] = {first->document_id, second->document_id};
    UmiDocumentSourceWorkspace *sources = NULL;
    UmiLanguageWorkspaceEditCatalogue *catalogue = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceWorkspaceCreate(documents, ids, 2U, &sources);
    char json[4096];
    int annotation = strncmp(mode, "annotation", 10U) == 0;
    const char *marker = annotation ? ",\"annotationId\":\"reason\"" : "";
    int length = snprintf(
        json, sizeof(json),
        "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"%s\",\"version\":null},\"edits\":[{\"range\":{"
        "\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":4}},\"newText\":\"saved\"%"
        "s},{\"range\":{\"start\":{\"line\":0,\"character\":5},\"end\":{\"line\":0,\"character\":9}},"
        "\"newText\":\"saved\"}]},{\"textDocument\":{\"uri\":\"%s\",\"version\":null},\"edits\":[{\"range\":{"
        "\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":4}},\"newText\":\"saved\"}]"
        "}]%s}",
        a.uri, marker, b.uri,
        annotation ? ",\"changeAnnotations\":{\"reason\":{\"label\":\"<b>Provider "
                     "reason</b>\",\"description\":\"A literal explanation\",\"needsConfirmation\":true}}"
                   : "");
    if (status == UMI_STATUS_OK && (length < 0 || (size_t)length >= sizeof(json)))
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(mode, "unchanged") == 0)
        (void)snprintf(json, sizeof(json), "{\"changes\":{\"%s\":[],\"%s\":[]}}", a.uri, b.uri);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageWorkspaceEditCatalogueCreate(json, strlen(json), NULL, &catalogue);
    if (status == UMI_STATUS_OK)
        status = UmiSourceWorkspaceEditReviewCreate(documents, &sources, &catalogue, NULL, NULL, out_model);
    UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    UmiDocumentSourceWorkspaceDestroy(sources);
    return status;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"apply",
                           "no-review",
                           "partial-review",
                           "no-approval",
                           "review-order",
                           "stale-first",
                           "stale-last",
                           "undo",
                           "busy",
                           "cancel",
                           "destroy",
                           "unbind",
                           "parent-close",
                           "creation-unbind",
                           "complete-unbind",
                           "owned-inputs",
                           "unchanged",
                           "retained-approval",
                           "annotation",
                           "annotation-missing",
                           "annotation-revoke",
                           "annotation-retained"};
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
    GtkWidget *apply = NULL, *approval = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    Completion result = {0};
    UmiSourceWorkspaceEditReview *model = NULL;
    UmiDocumentWorkingCopySnapshot first, second;
    UmiUiDocumentViewSnapshot view;
    application = gtk_application_new("org.umicom.workspace.edit.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.workspace.edit.native", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.workspace.edit.test", "Review", workbench, &shell) ==
          UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "first.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(documents, &first) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    CHECK(umi_ui_document_view_model_find(views, first.view_id, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "note NOTE", 9U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "second.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(documents, &second) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, second.view_id, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "note second", 11U) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Complete, &result) == UMI_STATUS_OK);
    CHECK(Model(documents, views, &first, &second, name, &model) == UMI_STATUS_OK);
    if (strcmp(name, "creation-unbind") == 0)
    {
        result.detach = adapter;
        gulong observer = g_signal_connect(gtk_window_get_toplevels(), "items-changed",
                                           G_CALLBACK(UnbindOnCreate), &result);
        UmiStatus status = UmiGtk4AdapterReviewWorkspaceEdits(adapter, &model);
        g_signal_handler_disconnect(gtk_window_get_toplevels(), observer);
        CHECK(status == UMI_STATUS_CANCELLED && model == NULL &&
              !UmiGtk4AdapterReplacementReviewBusy(adapter));
        dialog = Review();
        CHECK(dialog == NULL);
        goto unchanged;
    }
    CHECK(UmiGtk4AdapterReviewWorkspaceEdits(adapter, &model) == UMI_STATUS_OK && model == NULL);
    dialog = Review();
    CHECK(dialog != NULL && UmiGtk4AdapterReplacementReviewBusy(adapter));
    GtkWidget *button = Find(GTK_WIDGET(dialog), "document.workspace-edit.apply");
    CHECK(GTK_IS_BUTTON(button));
    apply = g_object_ref(button);
    button = Find(GTK_WIDGET(dialog), "document.workspace-edit.approval");
    CHECK(GTK_IS_CHECK_BUTTON(button));
    approval = g_object_ref(button);
    GtkWidget *mark = Find(GTK_WIDGET(dialog), "document.workspace-edit.reviewed"),
              *next = Find(GTK_WIDGET(dialog), "document.workspace-edit.next");
    GtkWidget *previous = Find(GTK_WIDGET(dialog), "document.workspace-edit.previous"),
              *close = Find(GTK_WIDGET(dialog), "document.workspace-edit.close");
    CHECK(GTK_IS_BUTTON(mark) && GTK_IS_BUTTON(next) && GTK_IS_BUTTON(previous) && GTK_IS_BUTTON(close));
    CHECK(!gtk_widget_get_sensitive(apply) && !gtk_widget_get_sensitive(approval) &&
          gtk_window_get_default_widget(dialog) == close);
    CHECK(TextEquals(views, first.view_id, "note NOTE") && TextEquals(views, second.view_id, "note second"));
    if (strcmp(name, "busy") == 0)
    {
        CHECK(UmiGtk4AdapterReviewReplacementSet(adapter, "a", "b") == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterReviewOpenReplacements(adapter, "a", "b") == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterDocumentSaveAll(adapter, NULL, NULL) == UMI_STATUS_BUSY);
        UmiDocumentReplacementProgress progress;
        CHECK(UmiGtk4AdapterReplacementReviewProgress(adapter, &progress) == UMI_STATUS_NOT_IMPLEMENTED);
        goto unchanged;
    }
    if (strcmp(name, "cancel") == 0)
    {
        CHECK(UmiGtk4AdapterCancelReplacementReview(adapter) == UMI_STATUS_OK);
        goto unchanged;
    }
    if (strcmp(name, "destroy") == 0 || strcmp(name, "retained-approval") == 0)
    {
        gtk_window_destroy(dialog);
        Pump();
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approval), TRUE);
        g_signal_emit_by_name(apply, "clicked");
        CHECK(!UmiGtk4AdapterReplacementReviewBusy(adapter));
        goto unchanged;
    }
    if (strcmp(name, "unbind") == 0)
    {
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        g_signal_emit_by_name(apply, "clicked");
        goto unchanged;
    }
    if (strcmp(name, "parent-close") == 0)
    {
        GtkWidget *parent = g_object_ref(umi_gtk4_adapter_native_window(adapter));
        gtk_window_destroy(GTK_WINDOW(parent));
        Pump();
        g_object_unref(parent);
        CHECK(!UmiGtk4AdapterReplacementReviewBusy(adapter));
        g_signal_emit_by_name(apply, "clicked");
        goto unchanged;
    }
    if (strcmp(name, "owned-inputs") == 0)
    {
        GtkWidget *details = Find(GTK_WIDGET(dialog), "document.workspace-edit.details");
        CHECK(GTK_IS_TEXT_VIEW(details) && !gtk_text_view_get_editable(GTK_TEXT_VIEW(details)));
        /* The temporary JSON buffer and input captures already retired in Model. */
        CHECK(model == NULL);
    }
    if (strcmp(name, "unchanged") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(mark));
        goto unchanged;
    }
    if (strcmp(name, "no-review") == 0)
    {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approval), TRUE);
        g_signal_emit_by_name(apply, "clicked");
        goto unchanged;
    }
    if (strcmp(name, "stale-first") == 0)
    {
        CHECK(umi_ui_document_view_model_find(views, first.view_id, &view) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "later", 5U) == UMI_STATUS_OK);
        g_signal_emit_by_name(mark, "clicked");
        g_signal_emit_by_name(apply, "clicked");
        goto unchanged;
    }
    if (strcmp(name, "review-order") == 0)
    {
        g_signal_emit_by_name(next, "clicked");
        g_signal_emit_by_name(mark, "clicked");
        g_signal_emit_by_name(previous, "clicked");
        g_signal_emit_by_name(mark, "clicked");
    }
    else
    {
        g_signal_emit_by_name(mark, "clicked");
        CHECK(!gtk_widget_get_sensitive(apply) && !gtk_widget_get_sensitive(approval));
        if (strcmp(name, "partial-review") == 0)
        {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(approval), TRUE);
            g_signal_emit_by_name(apply, "clicked");
            goto unchanged;
        }
        g_signal_emit_by_name(next, "clicked");
        g_signal_emit_by_name(mark, "clicked");
    }

    if (strncmp(name, "annotation", 10U) == 0)
    {
        GtkWidget *confirmation = Find(GTK_WIDGET(dialog), "document.workspace-edit.annotation.0");
        CHECK(GTK_IS_CHECK_BUTTON(confirmation) && !gtk_widget_get_sensitive(approval));
        GtkWidget *label = gtk_check_button_get_child(GTK_CHECK_BUTTON(confirmation));
        CHECK(GTK_IS_LABEL(label) && !gtk_label_get_use_markup(GTK_LABEL(label)) &&
              strstr(gtk_label_get_text(GTK_LABEL(label)), "<b>Provider reason</b>") != NULL);
        if (strcmp(name, "annotation-missing") == 0)
        {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(approval), TRUE);
            g_signal_emit_by_name(apply, "clicked");
            goto unchanged;
        }
        gtk_check_button_set_active(GTK_CHECK_BUTTON(confirmation), TRUE);
        CHECK(gtk_widget_get_sensitive(approval));
        if (strcmp(name, "annotation-revoke") == 0)
        {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(approval), TRUE);
            gtk_check_button_set_active(GTK_CHECK_BUTTON(confirmation), FALSE);
            CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(approval)) &&
                  !gtk_widget_get_sensitive(apply));
            g_signal_emit_by_name(apply, "clicked");
            goto unchanged;
        }
        if (strcmp(name, "annotation-retained") == 0)
        {
            g_object_ref(confirmation);
            gtk_window_destroy(dialog);
            Pump();
            gtk_check_button_set_active(GTK_CHECK_BUTTON(confirmation), FALSE);
            gtk_check_button_set_active(GTK_CHECK_BUTTON(confirmation), TRUE);
            gtk_check_button_set_active(GTK_CHECK_BUTTON(approval), TRUE);
            g_signal_emit_by_name(apply, "clicked");
            g_object_unref(confirmation);
            goto unchanged;
        }
    }
    CHECK(gtk_widget_get_sensitive(approval) && !gtk_widget_get_sensitive(apply));
    if (strcmp(name, "no-approval") == 0)
    {
        g_signal_emit_by_name(apply, "clicked");
        goto unchanged;
    }
    gtk_check_button_set_active(GTK_CHECK_BUTTON(approval), TRUE);
    CHECK(gtk_widget_get_sensitive(apply));
    if (strcmp(name, "stale-last") == 0)
    {
        CHECK(umi_ui_document_view_model_find(views, second.view_id, &view) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "later", 5U) == UMI_STATUS_OK);
    }
    if (strcmp(name, "complete-unbind") == 0)
        result.detach = adapter;
    g_signal_emit_by_name(apply, "clicked");
    if (strcmp(name, "stale-last") == 0)
        goto unchanged;
    CHECK(TextEquals(views, first.view_id, "saved saved") &&
          TextEquals(views, second.view_id, "saved second") && result.count == 1U);
    if (strcmp(name, "undo") == 0)
    {
        CHECK(UmiDocumentCoordinatorUndo(documents, first.document_id) == UMI_STATUS_OK);
        CHECK(TextEquals(views, first.view_id, "note NOTE") &&
              TextEquals(views, second.view_id, "saved second"));
    }
    if (strcmp(name, "complete-unbind") == 0)
    {
        CHECK(!UmiGtk4AdapterReplacementReviewBusy(adapter));
        g_signal_emit_by_name(apply, "clicked");
    }
    goto cleanup;
unchanged:
    CHECK(TextEquals(views, first.view_id, strcmp(name, "stale-first") == 0 ? "later" : "note NOTE"));
    CHECK(TextEquals(views, second.view_id, strcmp(name, "stale-last") == 0 ? "later" : "note second"));
    CHECK(result.count == 0U);
cleanup:
    UmiSourceWorkspaceEditReviewDestroy(model);
    if (adapter != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (dialog != NULL)
        gtk_window_destroy(dialog);
    g_clear_object(&dialog);
    g_clear_object(&apply);
    g_clear_object(&approval);
    umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    return failed;
}
