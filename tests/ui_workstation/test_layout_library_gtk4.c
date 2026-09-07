/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_library_gtk4.c
 *
 * PURPOSE:
 *   Exercise the real layout-library controls over the portable workspace
 *   owner, using unpresented native widgets and no files or product launches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/layout_library.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
    failed = 1; goto cleanup; } } while (0)

typedef struct LibraryFixture {
    UmiGtk4WorkspaceLayoutLibrary *library;
    UmiUiWorkspaceCustomisation *model;
    UmiUiWorkspaceLibraryPolicy policy;
    size_t reads;
    size_t applies;
    UmiStatus nested_status;
    UmiStatus read_failure;
    bool malformed_read;
    bool destroy_on_apply;
    bool destroy_on_read;
    char last_name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    char last_new_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    uint64_t last_expected_revision;
    bool last_confirmed;
    UmiGtk4WorkspaceLayoutLibraryStorageState storage;
    UmiGtk4WorkspaceLayoutLibraryStorageRequest last_storage_request;
    uint64_t actual_storage_revision;
    uint64_t actual_storage_generation;
    size_t storage_reads;
    size_t storage_calls;
    size_t saves;
    size_t restores;
    UmiStatus storage_read_failure;
    UmiStatus storage_operation_failure;
    bool malformed_storage_read;
    bool destroy_on_storage;
    bool destroy_on_storage_read;
} LibraryFixture;

/* This fixture reads cached owner evidence only; no DataServer/file is used. */
static UmiStatus read_storage(UmiGtk4WorkspaceLayoutLibraryStorageState *out, void *data)
{
    LibraryFixture *fixture = data;
    ++fixture->storage_reads;
    if (fixture->storage_read_failure != UMI_STATUS_OK) return fixture->storage_read_failure;
    *out = fixture->storage;
    if (fixture->malformed_storage_read) (void)memset(out->message, 'x', sizeof(out->message));
    if (fixture->destroy_on_storage_read) {
        umi_gtk4_ws_layout_library_destroy(fixture->library);
        fixture->library = NULL;
    }
    return UMI_STATUS_OK;
}

/* Simulate the owner's revision gates, not a second persistence implementation.
 * Separate core checkpoint fixtures verify actual storage and serialization. */
static UmiStatus operate_storage(const UmiGtk4WorkspaceLayoutLibraryStorageRequest *request, void *data)
{
    LibraryFixture *fixture = data;
    ++fixture->storage_calls;
    fixture->last_storage_request = *request;
    fixture->nested_status = umi_gtk4_ws_layout_library_refresh(fixture->library);
    if (request->expected_customisation_revision != fixture->model->revision ||
        request->expected_storage_generation != fixture->actual_storage_generation ||
        (request->action == UMI_GTK4_WORKSPACE_LAYOUT_LIBRARY_STORAGE_SAVE &&
         request->expected_storage_revision != fixture->actual_storage_revision)) {
        fixture->storage.save_enabled = false;
        return UMI_STATUS_INVALID_STATE;
    }
    if (fixture->storage_operation_failure != UMI_STATUS_OK) {
        /* A completed missing-library probe replaces old cached CAS evidence,
         * without replacing any layouts or counting as a successful restore. */
        if (fixture->storage_operation_failure == UMI_STATUS_NOT_FOUND) {
            fixture->actual_storage_revision = 0U;
            fixture->storage.storage_revision = 0U;
            fixture->storage.revision_known = true;
            fixture->storage.has_saved = false;
            fixture->storage.save_enabled = true;
        }
        return fixture->storage_operation_failure;
    }
    if (request->action == UMI_GTK4_WORKSPACE_LAYOUT_LIBRARY_STORAGE_RESTORE) {
        if (!request->restore_confirmed) return UMI_STATUS_PERMISSION_DENIED;
        ++fixture->restores;
        ++fixture->model->revision;
    } else {
        ++fixture->saves;
        ++fixture->actual_storage_revision;
    }
    fixture->storage.storage_revision = fixture->actual_storage_revision;
    fixture->storage.has_saved = true;
    fixture->storage.revision_known = true;
    fixture->storage.save_enabled = true;
    if (fixture->destroy_on_storage) {
        umi_gtk4_ws_layout_library_destroy(fixture->library);
        fixture->library = NULL;
    }
    return UMI_STATUS_OK;
}

/* Copy the one authoritative model, with explicit failure/lifetime injection. */
static UmiStatus read_library(UmiUiWorkspaceLibrarySnapshot *out, void *data)
{
    LibraryFixture *fixture = data;
    UmiStatus status;
    ++fixture->reads;
    if (fixture->read_failure != UMI_STATUS_OK) return fixture->read_failure;
    status = umi_ui_workspace_library_snapshot(fixture->model, &fixture->policy, out);
    if (fixture->malformed_read) out->layout_count = UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS + 1U;
    if (fixture->destroy_on_read) {
        umi_gtk4_ws_layout_library_destroy(fixture->library);
        fixture->library = NULL;
    }
    return status;
}

/* Apply through the existing portable model, never a second test registry. */
static UmiStatus apply_library(const UmiUiWorkspaceLibraryRequest *request, void *data)
{
    LibraryFixture *fixture = data;
    UmiStatus status;
    ++fixture->applies;
    fixture->last_expected_revision = request->expected_customisation_revision;
    fixture->last_confirmed = request->confirmed;
    (void)g_strlcpy(fixture->last_name, request->name, sizeof(fixture->last_name));
    (void)g_strlcpy(fixture->last_new_id, request->new_layout_id, sizeof(fixture->last_new_id));
    fixture->nested_status = umi_gtk4_ws_layout_library_refresh(fixture->library);
    status = umi_ui_workspace_library_apply(fixture->model, &fixture->policy, request, NULL);
    if (fixture->destroy_on_apply) {
        umi_gtk4_ws_layout_library_destroy(fixture->library);
        fixture->library = NULL;
    }
    return status;
}

/* Find actual native controls by their public automation identifiers. */
static GtkWidget *find_id(GtkWidget *widget, const char *id)
{
    if (widget == NULL) return NULL;
    if (g_strcmp0(g_object_get_data(G_OBJECT(widget), "umicom-automation-id"), id) == 0)
        return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_id(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Bound idle processing rather than running an application or display loop. */
static void drain_ready(void)
{
    for (size_t index = 0U; index < 128U && g_main_context_pending(NULL); ++index)
        (void)g_main_context_iteration(NULL, FALSE);
}

/* Resolve portable evidence by copied full identity. */
static const UmiUiWorkspaceLayout *layout_at(const LibraryFixture *fixture, const char *id)
{
    for (size_t index = 0U; index < fixture->model->layout_count; ++index)
        if (strcmp(fixture->model->layouts[index].layout_id, id) == 0)
            return &fixture->model->layouts[index];
    return NULL;
}

/* Optional storage remains explicit, revision-bound and safe during teardown. */
static int test_storage_controls(void)
{
    LibraryFixture fixture = {0};
    GtkWidget *root = NULL;
    GtkWidget *save;
    GtkWidget *restore;
    GtkWidget *confirm;
    GtkWidget *notice;
    GtkWidget *refresh;
    uint64_t model_revision;
    size_t calls;
    size_t storage_reads;
    int failed = 0;
    fixture.model = calloc(1U, sizeof(*fixture.model));
    CHECK(fixture.model != NULL);
    fixture.policy.layout_prefix = "test.layout.";
    umi_ui_workspace_customisation_init(fixture.model);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(fixture.model, "test.layout.storage", "Storage test") == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_layout_library_create(read_library, apply_library, &fixture, &fixture.library) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_ws_layout_library_popover(fixture.library));
    save = find_id(root, "workstation.layout-library.save-library");
    restore = find_id(root, "workstation.layout-library.restore-library");
    confirm = find_id(root, "workstation.layout-library.confirm-restore");
    notice = find_id(root, "workstation.layout-library.notice");
    refresh = find_id(root, "workstation.layout-library.refresh");
    CHECK(GTK_IS_BUTTON(save) && GTK_IS_BUTTON(restore) && GTK_IS_CHECK_BUTTON(confirm));
    CHECK(!gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(restore));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(notice)), "this session") != NULL);
    g_signal_emit_by_name(save, "clicked"); drain_ready(); CHECK(fixture.storage_calls == 0U);
    CHECK(umi_gtk4_ws_layout_library_set_storage_handlers(NULL, read_storage, operate_storage, &fixture) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_gtk4_ws_layout_library_set_storage_handlers(fixture.library, read_storage, NULL, &fixture) == UMI_STATUS_INVALID_ARGUMENT);
    fixture.storage.supported = true; fixture.storage.revision_known = true;
    fixture.storage.save_enabled = true; fixture.storage.restore_enabled = true;
    fixture.actual_storage_generation = 9U; fixture.storage.storage_generation = 9U;
    CHECK(umi_gtk4_ws_layout_library_set_storage_handlers(fixture.library, read_storage, operate_storage, &fixture) == UMI_STATUS_OK);
    CHECK(fixture.storage_calls == 0U && fixture.storage_reads == 1U);
    CHECK(gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(restore));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(notice)), "memory-only") != NULL);
    /* An inconclusive initial probe must not prevent an explicit retry. The
     * owner can return NOT_FOUND without replacing the current session list. */
    fixture.storage_operation_failure = UMI_STATUS_NOT_FOUND;
    model_revision = fixture.model->revision;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    CHECK(gtk_widget_get_sensitive(restore));
    g_signal_emit_by_name(restore, "clicked"); drain_ready();
    CHECK(fixture.storage_calls == 1U && fixture.restores == 0U && fixture.model->revision == model_revision);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(find_id(root, "workstation.layout-library.status"))), umi_status_text(UMI_STATUS_NOT_FOUND)) != NULL);
    /* A previously known archive can disappear. The owner's explicit probe
     * returns known zero, enabling Save immediately with the new cached CAS. */
    fixture.actual_storage_revision = 7U;
    fixture.storage.storage_revision = 7U;
    fixture.storage.has_saved = true;
    fixture.storage.save_enabled = false;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    CHECK(!gtk_widget_get_sensitive(save));
    storage_reads = fixture.storage_reads;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    g_signal_emit_by_name(restore, "clicked"); drain_ready();
    CHECK(fixture.storage_calls == 2U && fixture.restores == 0U);
    CHECK(fixture.last_storage_request.expected_storage_revision == 7U);
    CHECK(fixture.storage_reads == storage_reads + 1U);
    CHECK(fixture.model->revision == model_revision && fixture.model->layout_count == 1U);
    CHECK(strcmp(fixture.model->active_layout_id, "test.layout.storage") == 0);
    CHECK(gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(restore));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(find_id(root, "workstation.layout-library.status"))), umi_status_text(UMI_STATUS_NOT_FOUND)) != NULL);
    fixture.storage_operation_failure = UMI_STATUS_OK;
    calls = fixture.storage_calls;
    model_revision = fixture.model->revision;
    g_signal_emit_by_name(save, "clicked"); g_signal_emit_by_name(save, "clicked");
    CHECK(fixture.storage_calls == calls);
    CHECK(umi_gtk4_ws_layout_library_set_storage_handlers(fixture.library, NULL, NULL, NULL) == UMI_STATUS_BUSY);
    drain_ready();
    CHECK(fixture.saves == 1U && fixture.storage_calls == calls + 1U && fixture.nested_status == UMI_STATUS_BUSY);
    CHECK(fixture.last_storage_request.expected_customisation_revision == model_revision);
    CHECK(fixture.last_storage_request.expected_storage_revision == 0U && fixture.last_storage_request.expected_storage_generation == 9U);
    CHECK(!fixture.last_storage_request.restore_confirmed && !gtk_widget_get_sensitive(restore));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(find_id(root, "workstation.layout-library.status"))), "memory") != NULL);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    CHECK(gtk_widget_get_sensitive(restore));
    g_signal_emit_by_name(restore, "clicked");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), FALSE);
    drain_ready();
    CHECK(fixture.restores == 1U && fixture.last_storage_request.restore_confirmed);
    CHECK(fixture.model->revision == model_revision + 1U && !gtk_check_button_get_active(GTK_CHECK_BUTTON(confirm)));
    fixture.storage.durable = true;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(notice)), "for restart") != NULL);

    /* A stale save carries its original CAS. Only an explicit confirmed
     * Restore may acknowledge newer saved data; no automatic retry happens. */
    g_signal_emit_by_name(save, "clicked");
    ++fixture.actual_storage_revision;
    drain_ready();
    CHECK(fixture.saves == 1U && fixture.last_storage_request.expected_storage_revision == 1U);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(find_id(root, "workstation.layout-library.status"))), "Restore library") != NULL);
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    CHECK(!gtk_widget_get_sensitive(save));
    (void)g_strlcpy(fixture.storage.message, "Recovered the last good saved library.", sizeof(fixture.storage.message));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    g_signal_emit_by_name(restore, "clicked"); drain_ready();
    CHECK(fixture.restores == 2U && fixture.storage.storage_revision == fixture.actual_storage_revision);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(find_id(root, "workstation.layout-library.status"))), "last good") != NULL);
    fixture.storage.message[0] = '\0';

    /* A different backend may coincidentally expose the same CAS. Its
     * generation still rejects a queued request addressed to the old server. */
    calls = fixture.saves;
    g_signal_emit_by_name(save, "clicked");
    ++fixture.actual_storage_generation;
    fixture.storage.storage_generation = fixture.actual_storage_generation;
    drain_ready();
    CHECK(fixture.saves == calls && fixture.last_storage_request.expected_storage_generation == 9U);
    fixture.storage.save_enabled = true;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    model_revision = fixture.model->revision;
    g_signal_emit_by_name(save, "clicked");
    ++fixture.model->revision;
    drain_ready();
    CHECK(fixture.saves == calls && fixture.last_storage_request.expected_customisation_revision == model_revision);
    CHECK(fixture.model->revision == model_revision + 1U);
    fixture.storage.save_enabled = true;
    fixture.storage.revision_known = false;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    CHECK(!gtk_widget_get_sensitive(save) && gtk_widget_get_sensitive(confirm));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    CHECK(gtk_widget_get_sensitive(restore));
    g_signal_emit_by_name(restore, "clicked"); drain_ready();
    CHECK(fixture.restores == 3U && fixture.storage.revision_known);

    CHECK(umi_ui_workspace_customisation_begin_edit(fixture.model) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    CHECK(!gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(restore) && gtk_widget_get_sensitive(refresh));
    calls = fixture.storage_calls;
    g_signal_emit_by_name(save, "clicked"); drain_ready(); CHECK(fixture.storage_calls == calls);
    CHECK(umi_ui_workspace_customisation_cancel_edit(fixture.model) == UMI_STATUS_OK);
    fixture.storage_read_failure = UMI_STATUS_UNAVAILABLE;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_UNAVAILABLE);
    CHECK(!gtk_widget_get_sensitive(save));
    CHECK(gtk_widget_get_sensitive(find_id(root, "workstation.layout-library.rename")));
    fixture.storage_read_failure = UMI_STATUS_OK; fixture.malformed_storage_read = true;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_INVALID_ARGUMENT);
    fixture.malformed_storage_read = false; fixture.storage.supported = false;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    CHECK(!gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(confirm));
    fixture.storage.supported = true;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_layout_library_set_storage_handlers(fixture.library, NULL, NULL, NULL) == UMI_STATUS_OK);
    CHECK(!gtk_widget_get_sensitive(save) && strstr(gtk_label_get_text(GTK_LABEL(notice)), "this session") != NULL);
    CHECK(umi_gtk4_ws_layout_library_set_storage_handlers(fixture.library, read_storage, operate_storage, &fixture) == UMI_STATUS_OK);
    g_signal_emit_by_name(save, "clicked");
    calls = fixture.storage_calls;
    umi_gtk4_ws_layout_library_destroy(fixture.library); fixture.library = NULL;
    drain_ready();
    g_signal_emit_by_name(save, "clicked"); g_signal_emit_by_name(restore, "clicked");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE); drain_ready();
    CHECK(fixture.storage_calls == calls);
    g_clear_object(&root);

    CHECK(umi_gtk4_ws_layout_library_create(read_library, apply_library, &fixture, &fixture.library) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_layout_library_set_storage_handlers(fixture.library, read_storage, operate_storage, &fixture) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_ws_layout_library_popover(fixture.library));
    save = find_id(root, "workstation.layout-library.save-library");
    fixture.destroy_on_storage = true;
    g_signal_emit_by_name(save, "clicked"); drain_ready();
    CHECK(fixture.library == NULL && fixture.storage_calls == calls + 1U);
    g_signal_emit_by_name(save, "clicked"); drain_ready(); CHECK(fixture.storage_calls == calls + 1U);
    g_clear_object(&root);
    fixture.destroy_on_storage = false;
    CHECK(umi_gtk4_ws_layout_library_create(read_library, apply_library, &fixture, &fixture.library) == UMI_STATUS_OK);
    fixture.destroy_on_storage_read = true;
    CHECK(umi_gtk4_ws_layout_library_set_storage_handlers(fixture.library, read_storage, operate_storage, &fixture) == UMI_STATUS_CANCELLED);
    CHECK(fixture.library == NULL);
cleanup:
    umi_gtk4_ws_layout_library_destroy(fixture.library);
    if (root != NULL) g_object_unref(root);
    free(fixture.model);
    return failed;
}

/* Validate real actions, revision conflicts, input preservation and callback
 * teardown without presenting a popover, window, file dialog or application. */
int main(void)
{
    LibraryFixture fixture = {0};
    GtkWidget *root = NULL;
    GtkWidget *list;
    GtkWidget *search;
    GtkWidget *new_id;
    GtkWidget *name;
    GtkWidget *duplicate;
    GtkWidget *rename;
    GtkWidget *remove;
    GtkWidget *confirm;
    GtkWidget *refresh;
    GtkWidget *alpha;
    GtkWidget *beta;
    GtkWidget *copy;
    GtkWidget *held = NULL;
    UmiUiWorkspaceLibraryRequest external = {0};
    uint64_t revision;
    size_t calls;
    int start;
    int end;
    int failed = 0;

    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    CHECK(test_storage_controls() == 0);
    fixture.model = calloc(1U, sizeof(*fixture.model));
    CHECK(fixture.model != NULL);
    fixture.policy.layout_prefix = "test.layout.";
    umi_ui_workspace_customisation_init(fixture.model);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(fixture.model, "test.layout.alpha", "Alpha") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(fixture.model, "test.layout.beta", "Beta") == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_layout_library_create(NULL, apply_library, &fixture, &fixture.library) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_gtk4_ws_layout_library_create(read_library, apply_library, &fixture, &fixture.library) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_ws_layout_library_popover(fixture.library));
    CHECK(GTK_IS_POPOVER(root));
    CHECK(umi_gtk4_ws_layout_library_popover(NULL) == NULL);
    CHECK(umi_gtk4_ws_layout_library_refresh(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    list = find_id(root, "workstation.layout-library.list");
    search = find_id(root, "workstation.layout-library.search");
    new_id = find_id(root, "workstation.layout-library.new-id");
    name = find_id(root, "workstation.layout-library.name");
    duplicate = find_id(root, "workstation.layout-library.duplicate");
    rename = find_id(root, "workstation.layout-library.rename");
    remove = find_id(root, "workstation.layout-library.remove");
    confirm = find_id(root, "workstation.layout-library.confirm-remove");
    refresh = find_id(root, "workstation.layout-library.refresh");
    alpha = find_id(root, "workstation.layout-library.row.test.layout.alpha");
    beta = find_id(root, "workstation.layout-library.row.test.layout.beta");
    CHECK(GTK_IS_LIST_BOX(list) && GTK_IS_SEARCH_ENTRY(search) && GTK_IS_ENTRY(name));
    CHECK(GTK_IS_BUTTON(duplicate) && GTK_IS_BUTTON(rename) && GTK_IS_BUTTON(remove));
    CHECK(GTK_IS_CHECK_BUTTON(confirm) && GTK_IS_BUTTON(refresh));
    CHECK(GTK_IS_LIST_BOX_ROW(alpha) && GTK_IS_LIST_BOX_ROW(beta));
    CHECK(gtk_list_box_row_is_selected(GTK_LIST_BOX_ROW(beta)));
    CHECK(!gtk_widget_get_sensitive(remove) && !gtk_widget_get_sensitive(duplicate));
    gtk_editable_set_text(GTK_EDITABLE(search), "ALPHA");
    g_signal_emit_by_name(search, "search-changed");
    CHECK(gtk_widget_get_child_visible(alpha) && !gtk_widget_get_child_visible(beta));
    gtk_list_box_select_row(GTK_LIST_BOX(list), GTK_LIST_BOX_ROW(alpha));
    gtk_editable_set_text(GTK_EDITABLE(new_id), "test.layout.copy");
    gtk_editable_set_text(GTK_EDITABLE(name), "Draft \xCE\xBB");
    gtk_editable_select_region(GTK_EDITABLE(name), 1, 3);
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    CHECK(find_id(root, "workstation.layout-library.row.test.layout.alpha") == alpha);
    CHECK(find_id(root, "workstation.layout-library.name") == name);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(search)), "ALPHA") == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(name)), "Draft \xCE\xBB") == 0);
    CHECK(gtk_editable_get_selection_bounds(GTK_EDITABLE(name), &start, &end) && start == 1 && end == 3);
    revision = fixture.model->revision;
    g_signal_emit_by_name(duplicate, "clicked"); g_signal_emit_by_name(duplicate, "clicked");
    CHECK(fixture.applies == 0U && umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_BUSY);
    gtk_editable_set_text(GTK_EDITABLE(name), "Changed after queue");
    drain_ready();
    CHECK(fixture.applies == 1U && fixture.nested_status == UMI_STATUS_BUSY);
    CHECK(fixture.last_expected_revision == revision && strcmp(fixture.last_name, "Draft \xCE\xBB") == 0);
    CHECK(strcmp(fixture.last_new_id, "test.layout.copy") == 0 && layout_at(&fixture, "test.layout.copy") != NULL);
    CHECK(strcmp(layout_at(&fixture, "test.layout.copy")->name, "Draft \xCE\xBB") == 0);
    CHECK(strcmp(fixture.model->active_layout_id, "test.layout.copy") == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(name)), "Changed after queue") == 0);
    gtk_editable_set_text(GTK_EDITABLE(search), "");
    g_signal_emit_by_name(search, "search-changed");
    copy = find_id(root, "workstation.layout-library.row.test.layout.copy");
    CHECK(GTK_IS_LIST_BOX_ROW(copy) && gtk_list_box_row_is_selected(GTK_LIST_BOX_ROW(copy)));
    gtk_editable_set_text(GTK_EDITABLE(name), "Renamed");
    g_signal_emit_by_name(rename, "clicked"); drain_ready();
    CHECK(fixture.applies == 2U && strcmp(layout_at(&fixture, "test.layout.copy")->name, "Renamed") == 0);

    /* A concurrent portable edit must be rejected with the old seen revision,
     * not silently refreshed and retried against newly changed state. */
    external.action = UMI_UI_WORKSPACE_LIBRARY_RENAME;
    external.target_layout_id = "test.layout.copy"; external.name = "External rename";
    external.expected_customisation_revision = fixture.model->revision;
    CHECK(umi_ui_workspace_library_apply(fixture.model, &fixture.policy, &external, NULL) == UMI_STATUS_OK);
    revision = fixture.model->revision;
    gtk_editable_set_text(GTK_EDITABLE(name), "My stale draft");
    g_signal_emit_by_name(rename, "clicked"); drain_ready();
    CHECK(fixture.applies == 3U && fixture.model->revision == revision);
    CHECK(strcmp(layout_at(&fixture, "test.layout.copy")->name, "External rename") == 0);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(find_id(root, "workstation.layout-library.status"))), "Refresh") != NULL);
    CHECK(!gtk_widget_get_sensitive(rename) && gtk_widget_get_sensitive(refresh));
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(name)), "My stale draft") == 0);
    g_signal_emit_by_name(refresh, "clicked");
    CHECK(gtk_widget_get_sensitive(rename));

    /* Active edits block apply controls, while the catalogue remains readable. */
    CHECK(umi_ui_workspace_customisation_begin_edit(fixture.model) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);
    CHECK(!gtk_widget_get_sensitive(rename) && !gtk_widget_get_sensitive(duplicate));
    CHECK(!gtk_widget_get_sensitive(find_id(root, "workstation.layout-library.open")));
    CHECK(gtk_widget_get_sensitive(refresh) && gtk_widget_get_sensitive(search));
    g_signal_emit_by_name(rename, "clicked"); drain_ready(); CHECK(fixture.applies == 3U);
    CHECK(umi_ui_workspace_customisation_cancel_edit(fixture.model) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);

    /* Remove requires an explicit confirmation for the currently selected row. */
    CHECK(!gtk_widget_get_sensitive(remove));
    g_signal_emit_by_name(remove, "clicked"); drain_ready(); CHECK(fixture.applies == 3U);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    alpha = find_id(root, "workstation.layout-library.row.test.layout.alpha");
    gtk_list_box_select_row(GTK_LIST_BOX(list), GTK_LIST_BOX_ROW(alpha));
    CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(confirm)) && !gtk_widget_get_sensitive(remove));
    copy = find_id(root, "workstation.layout-library.row.test.layout.copy");
    gtk_list_box_select_row(GTK_LIST_BOX(list), GTK_LIST_BOX_ROW(copy));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    g_signal_emit_by_name(remove, "clicked");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), FALSE);
    drain_ready();
    CHECK(fixture.applies == 4U && fixture.last_confirmed && layout_at(&fixture, "test.layout.copy") == NULL);
    CHECK(fixture.model->layout_count == 2U && !gtk_check_button_get_active(GTK_CHECK_BUTTON(confirm)));
    beta = find_id(root, "workstation.layout-library.row.test.layout.beta");
    gtk_list_box_select_row(GTK_LIST_BOX(list), GTK_LIST_BOX_ROW(beta));
    g_signal_emit_by_name(find_id(root, "workstation.layout-library.open"), "clicked"); drain_ready();
    CHECK(fixture.applies == 5U && strcmp(fixture.model->active_layout_id, "test.layout.beta") == 0);
    alpha = find_id(root, "workstation.layout-library.row.test.layout.alpha");
    gtk_list_box_select_row(GTK_LIST_BOX(list), GTK_LIST_BOX_ROW(alpha));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    g_signal_emit_by_name(remove, "clicked"); drain_ready();
    CHECK(fixture.applies == 6U && fixture.model->layout_count == 1U);
    CHECK(!gtk_widget_get_sensitive(remove) && !gtk_widget_get_sensitive(confirm));

    /* A malformed/failed owner read preserves the visible rows and drafts but
     * prevents writes until an explicit successful refresh restores evidence. */
    beta = find_id(root, "workstation.layout-library.row.test.layout.beta");
    fixture.malformed_read = true;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(find_id(root, "workstation.layout-library.row.test.layout.beta") == beta);
    CHECK(!gtk_widget_get_sensitive(rename));
    fixture.malformed_read = false;
    fixture.read_failure = UMI_STATUS_UNAVAILABLE;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_UNAVAILABLE);
    fixture.read_failure = UMI_STATUS_OK;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_OK);

    /* Oversized UTF-8 input is rejected locally without silently truncating it. */
    {
        char oversized[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY + 8U];
        (void)memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        gtk_editable_set_text(GTK_EDITABLE(name), oversized);
        calls = fixture.applies;
        g_signal_emit_by_name(rename, "clicked"); drain_ready();
        CHECK(fixture.applies == calls && strcmp(gtk_editable_get_text(GTK_EDITABLE(name)), oversized) == 0);
    }
    gtk_editable_set_text(GTK_EDITABLE(name), "Never submitted");
    held = g_object_ref(rename);
    g_signal_emit_by_name(rename, "clicked");
    calls = fixture.applies;
    umi_gtk4_ws_layout_library_destroy(fixture.library); fixture.library = NULL;
    drain_ready();
    g_signal_emit_by_name(held, "clicked");
    gtk_editable_set_text(GTK_EDITABLE(search), "retained");
    g_signal_emit_by_name(search, "search-changed");
    g_signal_emit_by_name(refresh, "clicked"); drain_ready();
    CHECK(fixture.applies == calls);
    g_clear_object(&held); g_clear_object(&root);

    /* Rebuilding/destroying the manager inside Apply must not invalidate the
     * in-flight request or let later retained native callbacks reach the owner. */
    CHECK(umi_gtk4_ws_layout_library_create(read_library, apply_library, &fixture, &fixture.library) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_ws_layout_library_popover(fixture.library));
    fixture.destroy_on_apply = true;
    name = find_id(root, "workstation.layout-library.name");
    rename = find_id(root, "workstation.layout-library.rename");
    gtk_editable_set_text(GTK_EDITABLE(name), "Accepted before close");
    calls = fixture.applies;
    g_signal_emit_by_name(rename, "clicked"); drain_ready();
    CHECK(fixture.library == NULL && fixture.applies == calls + 1U);
    g_signal_emit_by_name(rename, "clicked"); drain_ready();
    CHECK(fixture.applies == calls + 1U);
    g_clear_object(&root);
    fixture.destroy_on_apply = false;
    CHECK(umi_gtk4_ws_layout_library_create(read_library, apply_library, &fixture, &fixture.library) == UMI_STATUS_OK);
    fixture.destroy_on_read = true;
    CHECK(umi_gtk4_ws_layout_library_refresh(fixture.library) == UMI_STATUS_CANCELLED);
    CHECK(fixture.library == NULL);
    fixture.destroy_on_read = false;
    umi_ui_workspace_customisation_init(fixture.model);
    CHECK(umi_gtk4_ws_layout_library_create(read_library, apply_library, &fixture, &fixture.library) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_ws_layout_library_popover(fixture.library));
    CHECK(!gtk_widget_get_sensitive(find_id(root, "workstation.layout-library.open")));
    CHECK(!gtk_widget_get_sensitive(find_id(root, "workstation.layout-library.duplicate")));
    CHECK(gtk_widget_get_sensitive(find_id(root, "workstation.layout-library.refresh")));

cleanup:
    umi_gtk4_ws_layout_library_destroy(fixture.library);
    if (held != NULL) g_object_unref(held);
    if (root != NULL) g_object_unref(root);
    free(fixture.model);
    return failed;
}
