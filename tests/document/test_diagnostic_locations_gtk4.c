/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_diagnostic_locations_gtk4.c
 * PURPOSE: Check explicit diagnostic destinations against current drafts, Unicode and retired UI ownership.
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
        if (Find(GTK_WIDGET(window), "document.completion.review") != NULL)
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

/* Retire a publication synchronously while GTK publishes its destination model.
 * This reproduces extension callbacks without depending on scheduling delays. */
static void RetireModel(GObject *control, GParamSpec *property, gpointer data)
{
    (void)property;
    if (gtk_drop_down_get_model(GTK_DROP_DOWN(control)) != NULL)
        gtk_editable_set_text(GTK_EDITABLE(data), "diagnostics-invalid");
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
static int ProgramMain(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {"open",
                           "primary",
                           "same-draft",
                           "related-draft",
                           "unicode",
                           "surrogate",
                           "outside",
                           "remote",
                           "authority",
                           "query",
                           "read-only",
                           "target-changed",
                           "target-shortened",
                           "primary-changed",
                           "primary-changed-back",
                           "caret-changed",
                           "invalid-location",
                           "invalid-diagnostic",
                           "choice-reset",
                           "no-related",
                           "changed-settings",
                           "worker-settings",
                           "retained",
                           "parent-close",
                           "unbind",
                           "literal",
                           "model-invalidate",
                           "selection-invalidate",
                           "open-unbind",
                           "hidden-apply"};
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
    GtkWindow *dialog = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    GtkWidget *retained_location = NULL, *retained_preview = NULL;
    Completion result = {0};
    char *directory = g_get_current_dir();
    UmiDocumentWorkingCopySnapshot primary, secondary;
    UmiUiDocumentViewSnapshot primary_view, secondary_view;
    const char *other =
        strcmp(mode, "unicode") == 0 || strcmp(mode, "surrogate") == 0 ? "a\n\xf0\x9f\x98\x80" : "a\nb";
    application = gtk_application_new("org.umicom.diagnostic.locations.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("diagnostic.locations", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.diagnostic.locations.test", "Diagnostics", workbench,
                                          &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    CHECK(umi_document_coordinator_new(documents, "main.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(documents, &primary) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, primary.view_id, &primary_view) == UMI_STATUS_OK);
    strcpy(primary_view.uri, "file:///workspace/main.c");
    strcpy(primary_view.language_id, "c");
    primary_view.dirty = 1;
    primary_view.cursor_offset = 2U;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &primary_view, "pu", 2U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "related.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(documents, &secondary) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, secondary.view_id, &secondary_view) == UMI_STATUS_OK);
    strcpy(secondary_view.uri, "file:///workspace/related.c");
    strcpy(secondary_view.language_id, "c");
    secondary_view.dirty = 1;
    secondary_view.read_only = strcmp(mode, "read-only") == 0;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &secondary_view, other, strlen(other)) == UMI_STATUS_OK);
/* Selecting only the view model left the workbench on the last created document. The workbench activation below updates both owners; retain the former fixture setup for review. */
#if 0
    CHECK(umi_ui_document_view_model_activate(views, primary.view_id) == UMI_STATUS_OK);
#endif
    /* Activate through the workbench so its coordinator and visible tab agree. */
    CHECK(umi_ui_workbench_activate_document(workbench, primary.view_id) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Complete, &result) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterReviewSourceDiagnostics(adapter, directory) == UMI_STATUS_OK);
    dialog = Review();
    CHECK(dialog != NULL);
    GtkWidget *program = Find(GTK_WIDGET(dialog), "document.completion.program"),
              *arguments = Find(GTK_WIDGET(dialog), "document.completion.arguments");
    GtkWidget *request = Find(GTK_WIDGET(dialog), "document.completion.request"),
              *preview = Find(GTK_WIDGET(dialog), "document.completion.preview");
    GtkWidget *choice = Find(GTK_WIDGET(dialog), "document.completion.choice"),
              *location = Find(GTK_WIDGET(dialog), "document.source-diagnostics.location");
    CHECK(GTK_IS_EDITABLE(program) && GTK_IS_EDITABLE(arguments) && GTK_IS_BUTTON(request) &&
          GTK_IS_BUTTON(preview));
    CHECK(GTK_IS_DROP_DOWN(choice) && GTK_IS_DROP_DOWN(location));
    CHECK(!gtk_widget_get_sensitive(location) && gtk_drop_down_get_model(GTK_DROP_DOWN(location)) == NULL);
    CHECK(UmiGtk4RecordingIsPrivate(location));
    if (strcmp(mode, "open") == 0)
        goto inspect;
    const char *peer = strcmp(mode, "unicode") == 0        ? "diagnostics-links-unicode"
                       : strcmp(mode, "outside") == 0      ? "diagnostics-links-outside"
                       : strcmp(mode, "remote") == 0       ? "diagnostics-links-remote"
                       : strcmp(mode, "authority") == 0    ? "diagnostics-links-authority"
                       : strcmp(mode, "query") == 0        ? "diagnostics-links-query"
                       : strcmp(mode, "literal") == 0      ? "diagnostics-links-literal"
                       : strcmp(mode, "choice-reset") == 0 ? "diagnostics-links-choices"
                       : strcmp(mode, "no-related") == 0   ? "diagnostics-links-none"
                                                           : "diagnostics-links-valid";
    gtk_editable_set_text(GTK_EDITABLE(program), argv[2]);
    gtk_editable_set_text(GTK_EDITABLE(arguments), peer);
    if (strcmp(mode, "model-invalidate") == 0)
        g_signal_connect(location, "notify::model", G_CALLBACK(RetireModel), arguments);
    g_signal_emit_by_name(request, "clicked");
    if (strcmp(mode, "worker-settings") == 0)
        gtk_editable_set_text(GTK_EDITABLE(arguments), "diagnostics-invalid");
    CHECK(Wait(GTK_WIDGET(dialog)));
    if (strcmp(mode, "worker-settings") == 0 || strcmp(mode, "model-invalidate") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(preview) && !gtk_widget_get_sensitive(location));
        CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(location)) == NULL);
        g_signal_emit_by_name(preview, "clicked");
        CHECK(result.count == 0U);
        goto inspect;
    }
    GListModel *destinations = gtk_drop_down_get_model(GTK_DROP_DOWN(location));
    CHECK(destinations != NULL);
    CHECK(g_list_model_get_n_items(destinations) == (strcmp(mode, "no-related") == 0 ? 1U : 3U));
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(location)) == 0U && gtk_widget_get_sensitive(preview));
    if (strcmp(mode, "literal") == 0)
    {
        const char *label = gtk_string_list_get_string(GTK_STRING_LIST(destinations), 2U);
        CHECK(strstr(label, "<b>declared & used</b>") != NULL);
        goto inspect;
    }
    guint destination = strcmp(mode, "primary") == 0 || strcmp(mode, "no-related") == 0 ? 0U
                        : strcmp(mode, "same-draft") == 0                               ? 1U
                                                                                        : 2U;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(location), destination);
    CHECK(result.count == 0U);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    CHECK(strcmp(active.view_id, primary.view_id) == 0);
    if (strcmp(mode, "choice-reset") == 0)
    {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(choice), 1U);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(location)) == 0U);
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(location))) == 1U);
        destination = 0U;
    }
    if (strcmp(mode, "target-changed") == 0)
        CHECK(UmiUiDocumentViewModelUpsertText(views, &secondary_view, "a\nz", 3U) == UMI_STATUS_OK);
    if (strcmp(mode, "target-shortened") == 0)
        CHECK(UmiUiDocumentViewModelUpsertText(views, &secondary_view, "a", 1U) == UMI_STATUS_OK);
    if (strcmp(mode, "primary-changed") == 0 || strcmp(mode, "primary-changed-back") == 0)
    {
        CHECK(UmiUiDocumentViewModelUpsertText(views, &primary_view, "new", 3U) == UMI_STATUS_OK);
        if (strcmp(mode, "primary-changed-back") == 0)
            CHECK(UmiUiDocumentViewModelUpsertText(views, &primary_view, "pu", 2U) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "caret-changed") == 0)
    {
        primary_view.cursor_offset = 1U;
        CHECK(umi_ui_document_view_model_upsert(views, &primary_view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "invalid-location") == 0)
        /* GTK may ignore an invalid index on a nonempty list. An empty model
         * exercises the no-selection path. Retain the former setup for review. */
#if 0
        gtk_drop_down_set_selected(GTK_DROP_DOWN(location), GTK_INVALID_LIST_POSITION);
#endif
        gtk_drop_down_set_model(GTK_DROP_DOWN(location), NULL);
    if (strcmp(mode, "invalid-diagnostic") == 0)
        /* GTK may ignore an invalid index on a nonempty list. An empty model
         * exercises the no-selection path. Retain the former setup for review. */
#if 0
        gtk_drop_down_set_selected(GTK_DROP_DOWN(choice), GTK_INVALID_LIST_POSITION);
#endif
        gtk_drop_down_set_model(GTK_DROP_DOWN(choice), NULL);
    if (strcmp(mode, "changed-settings") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(arguments), "diagnostics-invalid");
        CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(location)) == NULL &&
              !gtk_widget_get_sensitive(location));
    }
    if (strcmp(mode, "selection-invalidate") == 0)
    {
        g_signal_connect(location, "notify::selected", G_CALLBACK(RetireModel), arguments);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(location), 1U);
        CHECK(!gtk_widget_get_sensitive(preview));
    }
    if (strcmp(mode, "hidden-apply") == 0)
    {
        GtkWidget *apply = Find(GTK_WIDGET(dialog), "document.completion.apply");
        CHECK(GTK_IS_BUTTON(apply));
        g_signal_emit_by_name(apply, "clicked");
        CHECK(result.count == 0U);
        goto inspect;
    }
    if (strcmp(mode, "retained") == 0 || strcmp(mode, "parent-close") == 0 || strcmp(mode, "unbind") == 0)
    {
        retained_location = g_object_ref(location);
        retained_preview = g_object_ref(preview);
        if (strcmp(mode, "retained") == 0)
            gtk_window_destroy(dialog);
        else if (strcmp(mode, "parent-close") == 0)
            gtk_window_destroy(GTK_WINDOW(umi_gtk4_adapter_native_window(adapter)));
        else
            CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        unsigned before = result.count;
        gtk_drop_down_set_selected(GTK_DROP_DOWN(retained_location), 1U);
        g_signal_emit_by_name(retained_preview, "clicked");
        CHECK(result.count == before);
        goto inspect;
    }
    if (strcmp(mode, "open-unbind") == 0)
        result.detach = adapter;
    g_signal_emit_by_name(preview, "clicked");
    int refused = strcmp(mode, "surrogate") == 0 || strcmp(mode, "outside") == 0 ||
                  strcmp(mode, "remote") == 0 || strcmp(mode, "authority") == 0 ||
                  strcmp(mode, "query") == 0 || strcmp(mode, "target-shortened") == 0 ||
                  strcmp(mode, "primary-changed") == 0 || strcmp(mode, "primary-changed-back") == 0 ||
                  strcmp(mode, "caret-changed") == 0 || strcmp(mode, "invalid-location") == 0 ||
                  strcmp(mode, "invalid-diagnostic") == 0 || strcmp(mode, "changed-settings") == 0 ||
                  strcmp(mode, "selection-invalidate") == 0;
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    if (refused)
        CHECK(result.count == 0U && strcmp(active.view_id, primary.view_id) == 0);
    else
    {
        CHECK(result.count == 1U && result.status == UMI_STATUS_OK);
        UmiUiDocumentViewSnapshot opened;
        CHECK(umi_ui_document_view_model_find(views, active.view_id, &opened) == UMI_STATUS_OK);
        CHECK(strcmp(active.view_id, destination == 2U ? secondary.view_id : primary.view_id) == 0);
        CHECK(opened.cursor_offset == (destination == 2U                                        ? 2U
                                       : destination == 1U || strcmp(mode, "choice-reset") == 0 ? 1U
                                                                                                : 0U));
        CHECK(opened.selection_length == (strcmp(mode, "unicode") == 0                             ? 4U
                                          : destination == 0U && strcmp(mode, "choice-reset") != 0 ? 2U
                                                                                                   : 1U));
        CHECK(opened.dirty && (strcmp(mode, "read-only") != 0 || opened.read_only));
    }
inspect:
    CHECK(TextEquals(views, primary.view_id, strcmp(mode, "primary-changed") == 0 ? "new" : "pu"));
    CHECK(TextEquals(views, secondary.view_id,
                     strcmp(mode, "target-changed") == 0     ? "a\nz"
                     : strcmp(mode, "target-shortened") == 0 ? "a"
                                                             : other));
cleanup:
    if (adapter != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (dialog != NULL)
    {
        gtk_window_destroy(dialog);
        (void)Wait(GTK_WIDGET(dialog));
        g_object_unref(dialog);
    }
    g_clear_object(&retained_location);
    g_clear_object(&retained_preview);
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
