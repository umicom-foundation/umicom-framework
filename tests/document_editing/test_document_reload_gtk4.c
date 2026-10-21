/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_editing/test_document_reload_gtk4.c
 * PURPOSE: Exercise real saved-file review, refusal and native lifetime paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)
#define ORIGINAL "original source\n"
#define DRAFT "unsaved editor draft\n"
#define INCOMING "saved elsewhere\n"
#define LATER "later saved version\n"
typedef struct Result { unsigned count; UmiStatus status; UmiGtk4Adapter *adapter; int detach; } Result;
static void Completed(void *data, UmiStatus status)
{
    Result *result = data;
    ++result->count; result->status = status;
    if (result->detach) (void)UmiGtk4AdapterBindDocumentEditing(result->adapter, NULL, NULL, NULL);
}
static GtkWindow *FindForm(void)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(window), "document.reload.window") != NULL) return window;
        g_object_unref(window);
    }
    return NULL;
}
static int ViewText(UmiUiDocumentViewModel *views, const char *id, const char *expected)
{
    char *text = NULL; size_t length = 0U;
    UmiStatus status = UmiUiDocumentViewModelCopyText(views, id, &text, &length);
    int same = status == UMI_STATUS_OK && length == strlen(expected) && strcmp(text, expected) == 0;
    UmiUiDocumentViewModelFreeText(text); return same;
}
static int PaneText(GtkWindow *form, const char *id, const char *expected)
{
    GtkWidget *view = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), id);
    if (!GTK_IS_TEXT_VIEW(view) || gtk_text_view_get_editable(GTK_TEXT_VIEW(view))) return 0;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter start, end; gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    int same = strcmp(text, expected) == 0; g_free(text); return same;
}
typedef struct Observation {
    UmiGtk4Adapter *adapter;
    GtkWindow *parent, *observed;
    const char *mode;
    unsigned fired;
    gulong visible;
    UmiStatus nested;
} Observation;
static void Presented(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    Observation *observation = data;
    if (observation->fired || !gtk_widget_get_visible(GTK_WIDGET(object))) return;
    ++observation->fired; gtk_window_destroy(GTK_WINDOW(object));
}
static void Added(GListModel *model, guint position, guint removed, guint added, gpointer data)
{
    (void)removed;
    Observation *observation = data;
    if (observation->observed != NULL) return;
    for (guint i = 0U; i < added; ++i) {
        GtkWindow *window = g_list_model_get_item(model, position + i);
        if (window == observation->parent) { g_object_unref(window); continue; }
        observation->observed = window;
        if (strcmp(observation->mode, "construction-reenter") == 0) {
            ++observation->fired;
            observation->nested = UmiGtk4AdapterReviewDocumentReload(observation->adapter);
        } else if (strcmp(observation->mode, "construction-detach") == 0) {
            ++observation->fired;
            observation->nested = UmiGtk4AdapterBindDocumentEditing(observation->adapter, NULL, NULL, NULL);
        } else observation->visible = g_signal_connect(window, "notify::visible", G_CALLBACK(Presented), observation);
        return;
    }
}
static void CloseOnFeedback(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; gtk_window_destroy(GTK_WINDOW(data));
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *mode = argv[1];
    const char *cases[] = {"clean", "dirty", "cancel", "unapproved", "draft-changed", "disk-changed",
        "disk-missing", "other-tab", "undo", "detach", "retained-control", "retained-window",
        "reparented-control", "completion-detach", "construction-detach", "construction-reenter",
        "present-close", "feedback-close", "untitled", "busy"};
    int known = 0;
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(mode, cases[i]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    UmiCommandRegistry *commands = NULL; UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL; UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL; UmiGtk4Adapter *adapter = NULL;
    GtkApplication *application = NULL; GtkWindow *form = NULL, *foreign = NULL;
    GtkWidget *retained = NULL, *feedback = NULL;
    char *directory = g_build_filename(argv[2], mode, NULL), *source = g_build_filename(directory, "source.c", NULL);
    char *disk = NULL;
    char viewId[UMI_UI_ID_CAPACITY], otherId[UMI_UI_ID_CAPACITY];
    Result result = {0}; Observation observation = {0};
    GListModel *windows = NULL; gulong addedHandler = 0U, feedbackHandler = 0U;
    const int clean = strcmp(mode, "clean") == 0;
    const char *expected = clean ? ORIGINAL : DRAFT;
    const char *diskExpected = INCOMING;
    CHECK(g_mkdir_with_parents(directory, 0700) == 0);
    CHECK(g_file_set_contents(source, ORIGINAL, -1, NULL));
    application = gtk_application_new("org.umicom.reload.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("reload.gtk", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.reload.test", "Reload", workbench, &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_open(documents, source, viewId, sizeof viewId) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, viewId, &view) == UMI_STATUS_OK);
    if (!clean) {
        view.dirty = 1;
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, DRAFT, strlen(DRAFT)) == UMI_STATUS_OK);
    }
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK); result.adapter = adapter;
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Completed, &result) == UMI_STATUS_OK);
    CHECK(g_file_set_contents(source, INCOMING, -1, NULL));
    if (strcmp(mode, "untitled") == 0) {
        CHECK(umi_document_coordinator_new(documents, "untitled.c", otherId, sizeof otherId) == UMI_STATUS_OK);
        CHECK(UmiGtk4AdapterReviewDocumentReload(adapter) != UMI_STATUS_OK);
        CHECK(!UmiGtk4AdapterDocumentReloadBusy(adapter) && result.count == 0U); goto unchanged;
    }
    if (strncmp(mode, "construction-", 13U) == 0 || strcmp(mode, "present-close") == 0) {
        observation.adapter = adapter; observation.mode = mode;
        observation.parent = GTK_WINDOW(umi_gtk4_adapter_native_window(adapter));
        windows = gtk_window_get_toplevels();
        addedHandler = g_signal_connect(windows, "items-changed", G_CALLBACK(Added), &observation);
    }
    UmiStatus opened = UmiGtk4AdapterReviewDocumentReload(adapter);
    if (addedHandler != 0U) { g_signal_handler_disconnect(windows, addedHandler); addedHandler = 0U; }
    if (strcmp(mode, "construction-detach") == 0 || strcmp(mode, "present-close") == 0) {
        CHECK(opened == UMI_STATUS_CANCELLED && observation.fired == 1U);
        CHECK(!UmiGtk4WindowIsOpen(observation.observed) && !UmiGtk4AdapterDocumentReloadBusy(adapter));
        CHECK(result.count == (strcmp(mode, "present-close") == 0 ? 1U : 0U)); goto unchanged;
    }
    CHECK(opened == UMI_STATUS_OK);
    if (strcmp(mode, "construction-reenter") == 0) CHECK(observation.fired == 1U && observation.nested == UMI_STATUS_BUSY);
    form = FindForm(); CHECK(form != NULL);
    CHECK(PaneText(form, "umicom.comparison.left", expected));
    CHECK(PaneText(form, "umicom.comparison.right", INCOMING));
    GtkWidget *approve = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "document.reload.approve");
    GtkWidget *apply = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "document.reload.apply");
    GtkWidget *cancel = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "document.reload.cancel");
    CHECK(GTK_IS_CHECK_BUTTON(approve) && GTK_IS_BUTTON(apply) && GTK_IS_BUTTON(cancel));
    CHECK(!gtk_widget_get_sensitive(apply) && UmiGtk4AdapterDocumentReloadBusy(adapter));
    if (strcmp(mode, "busy") == 0) {
        CHECK(UmiGtk4AdapterReplacementReviewBusy(adapter));
        CHECK(UmiGtk4AdapterReviewDocumentReload(adapter) == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterPromptDocumentLocation(adapter) == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterReviewSelectedCode(adapter) == UMI_STATUS_BUSY); goto unchanged;
    }
    if (strcmp(mode, "unapproved") == 0) {
        g_signal_emit_by_name(apply, "clicked"); CHECK(result.count == 0U && UmiGtk4WindowIsOpen(form)); goto unchanged;
    }
    if (strcmp(mode, "cancel") == 0) {
        g_signal_emit_by_name(cancel, "clicked"); CHECK(result.count == 1U && result.status == UMI_STATUS_CANCELLED); goto unchanged;
    }
    gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE); CHECK(gtk_widget_get_sensitive(apply));
    if (strcmp(mode, "draft-changed") == 0 || strcmp(mode, "feedback-close") == 0) {
        CHECK(umi_ui_document_view_model_find(views, viewId, &view) == UMI_STATUS_OK); view.dirty = 1;
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, LATER, strlen(LATER)) == UMI_STATUS_OK); expected = LATER;
    }
    if (strcmp(mode, "disk-changed") == 0) { CHECK(g_file_set_contents(source, LATER, -1, NULL)); diskExpected = LATER; }
    if (strcmp(mode, "disk-missing") == 0) { CHECK(g_remove(source) == 0); diskExpected = NULL; }
    if (strcmp(mode, "other-tab") == 0)
        CHECK(umi_document_coordinator_new(documents, "other.c", otherId, sizeof otherId) == UMI_STATUS_OK);
    if (strcmp(mode, "detach") == 0) {
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        g_signal_emit_by_name(apply, "clicked"); CHECK(result.count == 0U); goto unchanged;
    }
    if (strcmp(mode, "retained-window") == 0 || strcmp(mode, "retained-control") == 0) {
        retained = g_object_ref(apply); gtk_window_destroy(form);
        if (strcmp(mode, "retained-control") == 0) g_clear_object(&form);
        g_signal_emit_by_name(retained, "clicked");
        CHECK(result.count == 1U && result.status == UMI_STATUS_CANCELLED);
        CHECK(!UmiGtk4AdapterDocumentReloadBusy(adapter)); goto unchanged;
    }
    if (strcmp(mode, "reparented-control") == 0) {
        retained = g_object_ref(apply); gtk_box_remove(GTK_BOX(gtk_widget_get_parent(apply)), apply);
        foreign = GTK_WINDOW(gtk_window_new()); g_object_ref(foreign);
        gtk_window_set_child(foreign, apply); gtk_window_present(foreign); g_signal_emit_by_name(apply, "clicked");
        CHECK(result.count == 0U && UmiGtk4WindowIsOpen(form)); goto unchanged;
    }
    if (strcmp(mode, "completion-detach") == 0) result.detach = 1;
    if (strcmp(mode, "feedback-close") == 0) {
        feedback = g_object_ref(umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "document.reload.message"));
        feedbackHandler = g_signal_connect(feedback, "notify::label", G_CALLBACK(CloseOnFeedback), form);
    }
    g_signal_emit_by_name(apply, "clicked");
    if (strcmp(mode, "draft-changed") == 0 || strcmp(mode, "disk-changed") == 0 ||
        strcmp(mode, "disk-missing") == 0 || strcmp(mode, "feedback-close") == 0) {
        CHECK(result.count == (strcmp(mode, "feedback-close") == 0 ? 1U : 0U)); goto unchanged;
    }
    CHECK(result.count == 1U && result.status == UMI_STATUS_OK);
    CHECK(!UmiGtk4AdapterDocumentReloadBusy(adapter)); expected = INCOMING;
    CHECK(umi_ui_document_view_model_find(views, viewId, &view) == UMI_STATUS_OK && !view.dirty);
    if (strcmp(mode, "other-tab") == 0) {
        UmiDocumentWorkingCopySnapshot active;
        CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
        CHECK(strcmp(active.view_id, otherId) == 0);
    }
    if (strcmp(mode, "undo") == 0) { CHECK(umi_document_coordinator_undo(documents) == UMI_STATUS_OK); expected = DRAFT; }
unchanged:
    CHECK(ViewText(views, viewId, expected));
    if (diskExpected != NULL) CHECK(g_file_get_contents(source, &disk, NULL, NULL) && strcmp(disk, diskExpected) == 0);
    else CHECK(!g_file_test(source, G_FILE_TEST_EXISTS));
cleanup:
    if (addedHandler != 0U) g_signal_handler_disconnect(windows, addedHandler);
    if (observation.visible != 0U) g_signal_handler_disconnect(observation.observed, observation.visible);
    if (feedbackHandler != 0U) g_signal_handler_disconnect(feedback, feedbackHandler);
    if (adapter != NULL) (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (form != NULL) { gtk_window_destroy(form); g_object_unref(form); }
    if (foreign != NULL) { gtk_window_destroy(foreign); g_object_unref(foreign); }
    g_clear_object(&observation.observed); g_clear_object(&retained); g_clear_object(&feedback);
    umi_gtk4_adapter_destroy(adapter); umi_document_coordinator_destroy(documents); umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell); umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands); g_clear_object(&application);
    g_free(disk); g_free(source); g_free(directory);
    return failed;
}
