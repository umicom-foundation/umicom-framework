/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_workspace_checkpoint_gtk4.c
 * Purpose: Verify explicit Suite layout checkpoints across native host restart.
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/gtk4_workstation.h"
#include "umicom/data/data_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; goto cleanup; \
    } \
} while (0)

/* No product, network or process service is started by this native fixture. */
static GtkWidget *create_panel(const UmiUiWorkspaceWindow *window, void *user_data)
{
    (void)user_data;
    return gtk_label_new(window->title);
}

/* Exercise the real button callback without relying on translated captions. */
static GtkWidget *find_tag(GtkWidget *root, const char *id)
{
    GtkWidget *child;
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_tag(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Prove the fixture does not present any native windows as a side effect. */
static int all_windows_unpresented(void)
{
    GListModel *windows = gtk_window_get_toplevels();
    guint index;
    for (index = 0U; index < g_list_model_get_n_items(windows); ++index) {
        GtkWindow *window = g_list_model_get_item(windows, index);
        int presented = gtk_widget_get_visible(GTK_WIDGET(window)) ||
            gtk_widget_get_mapped(GTK_WIDGET(window));
        g_object_unref(window);
        if (presented) return 0;
    }
    return 1;
}

/* Canonical and legacy product IDs must produce the same owned layout prefix. */
static int check_product_scopes(void)
{
    const char *ids[] = {"org.umicom.trader", "org.umicom.bank", "org.umicom.tms",
                        "org.umicom.music"};
    const char *canonical[] = {"org.umicom.trader", "org.umicom.bank", "org.umicom.tms",
                              "org.umicom.music-studio"};
    UmiApplicationSuiteGtk4Workstation *workstation = NULL;
    UmiDataServer *server = NULL;
    UmiApplicationSuiteGtk4WorkstationSnapshot snapshot;
    UmiUiLayoutPersistenceRecord *record = calloc(1U, sizeof(*record));
    char *text = calloc(UMI_UI_LAYOUT_ENCODED_CAPACITY, 1U);
    size_t index;
    int failed = 0;
    CHECK(record != NULL && text != NULL);
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    for (index = 0U; index < G_N_ELEMENTS(ids); ++index) {
        UmiApplicationSuiteGtk4WorkstationConfig config = {
            ids[index], "Checkpoint fixture", "Test", create_panel, NULL
        };
        CHECK(umi_application_suite_gtk4_workstation_create(&config, &workstation) == UMI_STATUS_OK);
        snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
        CHECK(!snapshot.checkpoint_storage_bound && !snapshot.has_saved_layout);
        CHECK(strcmp(snapshot.application_id, canonical[index]) == 0);
        CHECK(umi_application_suite_gtk4_workstation_export_layout(
            workstation, 7U, text, UMI_UI_LAYOUT_ENCODED_CAPACITY) == UMI_STATUS_OK);
        CHECK(umi_ui_layout_persistence_decode(text, record) == UMI_STATUS_OK);
        CHECK(strncmp(record->layout.layout_id, canonical[index], strlen(canonical[index])) == 0);
        CHECK(record->layout.layout_id[strlen(canonical[index])] == '.');
        CHECK(umi_application_suite_gtk4_workstation_bind_checkpoint_storage(
            workstation, server) == UMI_STATUS_OK);
        /* A prior product's save must never appear in the next product. */
        CHECK(!umi_application_suite_gtk4_workstation_snapshot(workstation).has_saved_layout);
        CHECK(umi_application_suite_gtk4_workstation_save_checkpoint(workstation, 8U) == UMI_STATUS_OK);
        CHECK(all_windows_unpresented());
        umi_application_suite_gtk4_workstation_destroy(workstation);
        workstation = NULL;
    }
cleanup:
    umi_application_suite_gtk4_workstation_destroy(workstation);
    umi_data_server_destroy(server);
    free(record);
    free(text);
    return failed;
}

/* Both memory and SQLite borrow the same server contract. Only the SQLite
 * journey closes and reopens the actual temporary database between hosts. */
static int check_restart(UmiDataServer **server, const char *sqlite_path, int durable)
{
    UmiApplicationSuiteGtk4WorkstationConfig config = {
        "org.umicom.trader", "Checkpoint fixture", "Test", create_panel, NULL
    };
    UmiApplicationSuiteGtk4Workstation *first = NULL;
    UmiApplicationSuiteGtk4Workstation *second = NULL;
    UmiApplicationSuiteGtk4WorkstationSnapshot snapshot;
    UmiUiLayoutPersistenceRecord *record = calloc(1U, sizeof(*record));
    char *text = calloc(UMI_UI_LAYOUT_ENCODED_CAPACITY, 1U);
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char sentinel[32];
    GtkWidget *save_button;
    UmiUiWorkspacePanelSettings settings;
    uint64_t saved_revision;
    int failed = 0;
    CHECK(record != NULL && text != NULL);
    CHECK(umi_application_suite_gtk4_workstation_create(&config, &first) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_bind_checkpoint_storage(first, *server) == UMI_STATUS_OK);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(first);
    CHECK(snapshot.checkpoint_storage_bound && !snapshot.has_saved_layout);
    CHECK(snapshot.checkpoint_storage_durable == durable);
    CHECK(snapshot.checkpoint_storage_revision == 0U);
    CHECK(umi_application_suite_gtk4_workstation_create_blank_layout(
        first, "org.umicom.trader.checkpoint-canvas", "Saved canvas") == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_begin_layout_edit(first) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_bind_checkpoint_storage(first, *server) == UMI_STATUS_BUSY);
    CHECK(umi_application_suite_gtk4_workstation_save_checkpoint(first, 9U) == UMI_STATUS_BUSY);
    CHECK(umi_application_suite_gtk4_workstation_restore_checkpoint(first) == UMI_STATUS_BUSY);
    CHECK(umi_application_suite_gtk4_workstation_open_window(
        first, "chart", "canvas", 0, 10U, window_id, sizeof(window_id)) == UMI_STATUS_OK);
    settings = umi_ui_workspace_panel_settings_default(window_id);
    settings.placement_id = "canvas";
    settings.stack_id = "canvas";
    settings.x = 0.125; settings.y = 0.25; settings.width = 0.5; settings.height = 0.5;
    CHECK(umi_application_suite_gtk4_workstation_apply_panel_settings(first, &settings) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_commit_layout_edit(first) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_save_checkpoint(first, 1234U) == UMI_STATUS_OK);
    /* A borrowed transaction must not be committed or interpreted as damaged
     * checkpoint metadata. Both commands remain retryable after rollback. */
    CHECK(umi_data_server_begin(*server) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_save_checkpoint(first, 999U) == UMI_STATUS_BUSY);
    CHECK(umi_application_suite_gtk4_workstation_restore_checkpoint(first) == UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(*server));
    save_button = find_tag(umi_application_suite_gtk4_workstation_widget(first), "umicom.layout.save");
    CHECK(save_button != NULL && gtk_widget_get_sensitive(save_button));
    CHECK(umi_data_server_rollback(*server) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_save_checkpoint(first, 1234U) == UMI_STATUS_OK);
    saved_revision = umi_application_suite_gtk4_workstation_snapshot(first).checkpoint_storage_revision;
    CHECK(saved_revision > 0U);
    /* A later unsaved layout is deliberately not an implicit autosave. */
    CHECK(umi_application_suite_gtk4_workstation_create_blank_layout(
        first, "org.umicom.trader.unsaved-canvas", "Unsaved canvas") == UMI_STATUS_OK);
    umi_application_suite_gtk4_workstation_destroy(first);
    first = NULL;
    CHECK(umi_data_server_set(*server, "checkpoint-fixture.sentinel", "borrowed") == UMI_STATUS_OK);
    if (sqlite_path != NULL) {
        umi_data_server_destroy(*server);
        *server = NULL;
        CHECK(umi_data_server_create_sqlite(sqlite_path, server) == UMI_STATUS_OK);
    }
    CHECK(umi_data_server_get(*server, "checkpoint-fixture.sentinel", sentinel, sizeof(sentinel)) == UMI_STATUS_OK);
    CHECK(strcmp(sentinel, "borrowed") == 0);
    CHECK(umi_application_suite_gtk4_workstation_create(&config, &first) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_bind_checkpoint_storage(first, *server) == UMI_STATUS_OK);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(first);
    CHECK(snapshot.has_saved_layout && snapshot.checkpoint_storage_durable == durable);
    CHECK(strcmp(snapshot.active_layout_id, "checkpoint-canvas") != 0);
    CHECK(strcmp(snapshot.active_layout_id, "org.umicom.trader.checkpoint-canvas") != 0);
    CHECK(umi_application_suite_gtk4_workstation_restore_checkpoint(first) == UMI_STATUS_OK);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(first);
    CHECK(strcmp(snapshot.active_layout_id, "org.umicom.trader.checkpoint-canvas") == 0);
    CHECK(snapshot.canvas_panel_count == 1U && snapshot.layout_locked);
    CHECK(snapshot.saved_layout_at_ns == 1234U && snapshot.checkpoint_storage_revision == saved_revision);
    CHECK(umi_application_suite_gtk4_workstation_export_layout(
        first, 42U, text, UMI_UI_LAYOUT_ENCODED_CAPACITY) == UMI_STATUS_OK);
    CHECK(umi_ui_layout_persistence_decode(text, record) == UMI_STATUS_OK);
    CHECK(record->layout.window_count == 1U);
    CHECK(strcmp(record->layout.windows[0].placement_id, "canvas") == 0);
    CHECK(record->layout.windows[0].x == 0.125 && record->layout.windows[0].y == 0.25);
    CHECK(record->layout.windows[0].width == 0.5 && record->layout.windows[0].height == 0.5);

    /* Competing windows cannot silently overwrite a more recent explicit save. */
    CHECK(umi_application_suite_gtk4_workstation_create(&config, &second) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_bind_checkpoint_storage(second, *server) == UMI_STATUS_OK);
    save_button = find_tag(umi_application_suite_gtk4_workstation_widget(first), "umicom.layout.save");
    CHECK(save_button != NULL && gtk_widget_get_sensitive(save_button));
    g_signal_emit_by_name(save_button, "clicked");
    snapshot = umi_application_suite_gtk4_workstation_snapshot(first);
    CHECK(snapshot.checkpoint_storage_status == UMI_STATUS_OK);
    CHECK(snapshot.checkpoint_storage_revision > saved_revision);
    CHECK(umi_application_suite_gtk4_workstation_save_checkpoint(second, 4567U) != UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_snapshot(second).checkpoint_storage_revision == saved_revision);
    CHECK(umi_application_suite_gtk4_workstation_restore_checkpoint(second) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_snapshot(second).checkpoint_storage_revision ==
        snapshot.checkpoint_storage_revision);
    /* Explicit unbinding is the only route back to session-only checkpoints. */
    CHECK(umi_application_suite_gtk4_workstation_bind_checkpoint_storage(second, NULL) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_save_checkpoint(second, 5678U) == UMI_STATUS_OK);
    CHECK(!umi_application_suite_gtk4_workstation_snapshot(second).checkpoint_storage_bound);
    CHECK(all_windows_unpresented());
cleanup:
    umi_application_suite_gtk4_workstation_destroy(second);
    umi_application_suite_gtk4_workstation_destroy(first);
    free(record);
    free(text);
    return failed;
}

/* No windows are presented. The SQLite case uses only a fixture-owned temporary
 * directory, never user configuration or any product's real Data Server. */
int main(void)
{
    UmiDataServer *server = NULL;
    char *directory = NULL;
    char *path = NULL;
    GError *error = NULL;
    UmiStatus status;
    int failed = 0;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    CHECK(check_product_scopes() == 0);
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(check_restart(&server, NULL, 0) == 0);
    umi_data_server_destroy(server);
    server = NULL;
    directory = g_dir_make_tmp("umicom-suite-checkpoint-XXXXXX", &error);
    CHECK(directory != NULL);
    path = g_build_filename(directory, "workspace.sqlite3", NULL);
    status = umi_data_server_create_sqlite(path, &server);
    CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_UNAVAILABLE);
    if (status == UMI_STATUS_OK) CHECK(check_restart(&server, path, 1) == 0);
    else {
        (void)fprintf(stderr, "SKIP: SQLite unavailable; memory checks completed, durable restart unverified.\n");
        failed = 77;
    }
cleanup:
    umi_data_server_destroy(server);
    /* Keep the isolated fixture for diagnosis; no test or user files are removed. */
    if (directory != NULL)
        (void)fprintf(stderr, "Retained checkpoint fixture: %s\n", directory);
    g_clear_error(&error);
    g_free(path);
    g_free(directory);
    return failed;
}
