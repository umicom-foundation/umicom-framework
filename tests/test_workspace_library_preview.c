/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_library_preview.c
 * PURPOSE: Verify copied comparison and read-only saved-library review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_library_checkpoint.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef struct Fixture {
    UmiUiWorkspaceCustomisation model, before;
    UmiUiWorkspaceLibrarySnapshot current, proposed, saved;
    UmiUiWorkspaceLibraryComparison difference, difference_before;
    UmiUiWorkspaceLibraryPreview preview, preview_before;
} Fixture;
static const UmiUiWorkspaceLibraryPolicy policy = {"test.preview."};
static const UmiUiWorkspaceCheckpointScope scope = {"org.umicom.preview-test", "test", "test.preview."};

static UmiUiWorkspaceLibraryRow Row(const char *id, const char *name, bool active)
{
    UmiUiWorkspaceLibraryRow row = {0};
    (void)snprintf(row.layout_id, sizeof(row.layout_id), "%s", id);
    (void)snprintf(row.name, sizeof(row.name), "%s", name);
    row.active = active; row.locked = true; row.window_count = 2U;
    return row;
}
static int CompareRejected(Fixture *f, UmiStatus expected)
{
    memset(&f->difference, 0x5a, sizeof(f->difference));
    memcpy(&f->difference_before, &f->difference, sizeof(f->difference));
    CHECK(umi_ui_workspace_library_compare(&f->current, &f->proposed, &f->difference) == expected);
    CHECK(memcmp(&f->difference, &f->difference_before, sizeof(f->difference)) == 0);
    return 0;
}
static int Seed(Fixture *f, UmiDataServer *server)
{
    umi_ui_workspace_customisation_init(&f->model);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(&f->model, "test.preview.a", "Saved A") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(&f->model, "test.preview.b", "Saved B") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_library_checkpoint_save(server, &scope, &f->model, 100U, 0U, NULL) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_library_snapshot(&f->model, &policy, &f->saved) == UMI_STATUS_OK);
    return 0;
}
static int Run(Fixture *f, UmiDataServer *server, const char *name)
{
    f->current.rows[0] = Row("a", "Alpha", true);
    f->current.rows[1] = Row("b", "Beta", false);
    f->current.layout_count = 2U; f->current.customisation_revision = 7U;
    f->proposed = f->current; f->proposed.customisation_revision = 9U;
    if (strcmp(name, "compare") == 0) {
        f->proposed.rows[0] = Row("b", "Renamed beta", true);
        f->proposed.rows[0].window_count = 4U; f->proposed.rows[0].locked = false;
        f->proposed.rows[1] = Row("c", "Added", false);
        CHECK(umi_ui_workspace_library_compare(&f->current, &f->proposed, &f->difference) == UMI_STATUS_OK);
        CHECK(f->difference.row_count == 3U && f->difference.added_count == 1U && f->difference.removed_count == 1U && f->difference.changed_count == 1U);
        CHECK(f->difference.current_revision == 7U && f->difference.proposed_revision == 9U);
        CHECK(f->difference.rows[0].before_index == 1U && f->difference.rows[0].after_index == 0U);
        CHECK(f->difference.rows[0].changes == (UMI_UI_WORKSPACE_LIBRARY_CHANGE_NAME | UMI_UI_WORKSPACE_LIBRARY_CHANGE_POSITION |
            UMI_UI_WORKSPACE_LIBRARY_CHANGE_WINDOWS | UMI_UI_WORKSPACE_LIBRARY_CHANGE_LOCKED | UMI_UI_WORKSPACE_LIBRARY_CHANGE_ACTIVE));
        CHECK(f->difference.rows[1].before_index == SIZE_MAX && f->difference.rows[1].changes == UMI_UI_WORKSPACE_LIBRARY_CHANGE_ADDED);
        CHECK(f->difference.rows[2].after_index == SIZE_MAX && strcmp(f->difference.rows[2].before.layout_id, "a") == 0);
        f->proposed.rows[0].name[0] = 'X';
        CHECK(strcmp(f->difference.rows[0].after.name, "Renamed beta") == 0);
    } else if (strcmp(name, "unchanged") == 0) {
        f->proposed.customisation_revision = UINT64_MAX;
        CHECK(umi_ui_workspace_library_compare(&f->current, &f->proposed, &f->difference) == UMI_STATUS_OK);
        CHECK(f->difference.row_count == 2U && f->difference.changed_count == 0U);
        CHECK(f->difference.rows[0].changes == 0U && f->difference.rows[1].changes == 0U);
        CHECK(f->current.customisation_revision == 7U && f->proposed.customisation_revision == UINT64_MAX);
    } else if (strcmp(name, "empty") == 0) {
        f->proposed.layout_count = 0U;
        CHECK(umi_ui_workspace_library_compare(&f->current, &f->proposed, &f->difference) == UMI_STATUS_OK && f->difference.removed_count == 2U);
        f->current.layout_count = 0U;
        CHECK(umi_ui_workspace_library_compare(&f->current, &f->proposed, &f->difference) == UMI_STATUS_OK && f->difference.row_count == 0U);
        f->proposed.layout_count = 2U;
        CHECK(umi_ui_workspace_library_compare(&f->current, &f->proposed, &f->difference) == UMI_STATUS_OK && f->difference.added_count == 2U);
    } else if (strcmp(name, "invalid") == 0) {
        UmiUiWorkspaceLibrarySnapshot good = f->proposed;
        f->proposed.layout_count = UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS + 1U;
        CHECK(CompareRejected(f, UMI_STATUS_INVALID_STATE) == 0);
        f->proposed = good; memset(f->proposed.rows[1].name, 'x', sizeof(f->proposed.rows[1].name));
        CHECK(CompareRejected(f, UMI_STATUS_INVALID_STATE) == 0);
        f->proposed = good; strcpy(f->proposed.rows[1].layout_id, "a");
        CHECK(CompareRejected(f, UMI_STATUS_INVALID_STATE) == 0);
        f->proposed = good; f->proposed.rows[1].active = true;
        CHECK(CompareRejected(f, UMI_STATUS_INVALID_STATE) == 0);
        f->proposed = good; f->proposed.rows[0].active = false;
        CHECK(CompareRejected(f, UMI_STATUS_INVALID_STATE) == 0);
        f->proposed = good; strcpy(f->proposed.rows[1].name, "\xed\xa0\x80");
        CHECK(CompareRejected(f, UMI_STATUS_INVALID_STATE) == 0);
        f->proposed = good; strcpy(f->proposed.rows[1].layout_id, "bad/id");
        CHECK(CompareRejected(f, UMI_STATUS_INVALID_STATE) == 0);
        f->proposed = good; f->proposed.rows[1].window_count = UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS + 1U;
        CHECK(CompareRejected(f, UMI_STATUS_INVALID_STATE) == 0);
        f->proposed = good; f->current.editing = true;
        CHECK(CompareRejected(f, UMI_STATUS_BUSY) == 0);
        f->current.editing = false; f->proposed.editing = true;
        CHECK(CompareRejected(f, UMI_STATUS_BUSY) == 0);
    } else if (strcmp(name, "capacity") == 0) {
        f->current.layout_count = f->proposed.layout_count = UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS;
        for (size_t i = 0U; i < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS; ++i) {
            char id[64]; (void)snprintf(id, sizeof(id), "old.%zu", i);
            f->current.rows[i] = Row(id, "Old", i == 0U);
            (void)snprintf(id, sizeof(id), "new.%zu", i);
            f->proposed.rows[i] = Row(id, "New", i == 0U);
        }
        CHECK(umi_ui_workspace_library_compare(&f->current, &f->proposed, &f->difference) == UMI_STATUS_OK);
        CHECK(f->difference.row_count == UMI_UI_WORKSPACE_LIBRARY_MAX_COMPARISON_ROWS);
        CHECK(f->difference.added_count == UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS && f->difference.removed_count == UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS);
    } else if (strcmp(name, "alias") == 0) {
        CHECK(umi_ui_workspace_library_compare(NULL, &f->proposed, &f->difference) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_workspace_library_compare(&f->current, NULL, &f->difference) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_workspace_library_compare(&f->current, &f->proposed, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_workspace_library_compare(&f->current, &f->current, &f->difference) == UMI_STATUS_OK);
        CHECK(f->difference.changed_count == 0U);
        f->saved = f->current;
        CHECK(umi_ui_workspace_library_compare(&f->current, &f->proposed,
            (UmiUiWorkspaceLibraryComparison *)(void *)&f->current) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&f->current, &f->saved, sizeof(f->current)) == 0);
        memset(&f->preview, 0x5a, sizeof(f->preview));
        memcpy(&f->preview_before, &f->preview, sizeof(f->preview));
        CHECK(umi_ui_workspace_library_checkpoint_preview(NULL, &scope, &f->model, &f->preview) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_workspace_library_checkpoint_preview(server, NULL, &f->model, &f->preview) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_workspace_library_checkpoint_preview(server, &scope, NULL, &f->preview) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_workspace_library_checkpoint_preview(server, &scope, &f->model, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&f->preview, &f->preview_before, sizeof(f->preview)) == 0);
        f->before = f->model;
        CHECK(umi_ui_workspace_library_checkpoint_preview(server, &scope, &f->model,
            (UmiUiWorkspaceLibraryPreview *)(void *)&f->model) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
    } else if (strcmp(name, "missing") == 0) {
        umi_ui_workspace_customisation_init(&f->model);
        CHECK(umi_ui_workspace_customisation_create_blank_layout(&f->model, "test.preview.a", "Local") == UMI_STATUS_OK);
        memset(&f->preview, 0x5a, sizeof(f->preview)); memcpy(&f->preview_before, &f->preview, sizeof(f->preview)); f->before = f->model;
        CHECK(umi_ui_workspace_library_checkpoint_preview(server, &scope, &f->model, &f->preview) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&f->preview, &f->preview_before, sizeof(f->preview)) == 0 && memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
        CHECK(umi_data_server_count(server) == 0U);
    } else {
        CHECK(Seed(f, server) == 0);
        UmiUiWorkspaceLibraryRequest rename = {UMI_UI_WORKSPACE_LIBRARY_RENAME, "test.preview.a", NULL, "Local A", f->model.revision, false};
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &rename, NULL) == UMI_STATUS_OK);
        f->before = f->model;
        size_t records = umi_data_server_count(server);
        if (strcmp(name, "preview") == 0 || strcmp(name, "stale-save") == 0) {
            if (strcmp(name, "stale-save") == 0)
                CHECK(umi_ui_workspace_library_checkpoint_save(server, &scope, &f->model, 200U, 1U, NULL) == UMI_STATUS_OK);
            records = umi_data_server_count(server);
            CHECK(umi_ui_workspace_library_checkpoint_preview(server, &scope, &f->model, &f->preview) == UMI_STATUS_OK);
            CHECK(!f->preview.report.checkpoint.durable && !f->preview.report.checkpoint.recovered_last_good);
            CHECK(f->preview.saved.layout_count == 2U);
            if (strcmp(name, "preview") == 0) {
                CHECK(f->preview.comparison.changed_count == 1U);
                CHECK(strcmp(f->preview.saved.rows[0].name, "Saved A") == 0);
                CHECK(f->preview.report.checkpoint.storage_revision == 1U);
            } else {
                CHECK(f->preview.report.checkpoint.storage_revision == 2U);
                CHECK(umi_ui_workspace_library_checkpoint_save(server, &scope, &f->model, 300U, 1U, NULL) == UMI_STATUS_INVALID_STATE);
            }
        } else if (strcmp(name, "recovery") == 0) {
            char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
            CHECK(umi_ui_workspace_library_checkpoint_save(server, &scope, &f->model, 200U, 1U, NULL) == UMI_STATUS_OK);
            CHECK(umi_workbench_layout_data_key_build(UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK,
                "org.umicom.preview-test@test@library-primary", NULL, 2U, 0U, key, sizeof(key)) == UMI_STATUS_OK);
            CHECK(umi_data_server_set(server, key, "corrupt") == UMI_STATUS_OK);
            records = umi_data_server_count(server);
            CHECK(umi_ui_workspace_library_checkpoint_preview(server, &scope, &f->model, &f->preview) == UMI_STATUS_OK);
            CHECK(f->preview.report.checkpoint.recovered_last_good && f->preview.report.checkpoint.storage_revision == 2U);
            CHECK(strcmp(f->preview.saved.rows[0].name, "Saved A") == 0);
            /* If both copies are unusable, keep the last successful preview
             * as caller-owned output and never repair the saved data here. */
            memcpy(&f->preview_before, &f->preview, sizeof(f->preview));
            CHECK(umi_workbench_layout_data_key_build(UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK,
                "org.umicom.preview-test@test@library-last-good", NULL, 1U, 0U, key, sizeof(key)) == UMI_STATUS_OK);
            CHECK(umi_data_server_set(server, key, "corrupt backup") == UMI_STATUS_OK);
            CHECK(umi_ui_workspace_library_checkpoint_preview(server, &scope, &f->model, &f->preview) != UMI_STATUS_OK);
            CHECK(memcmp(&f->preview, &f->preview_before, sizeof(f->preview)) == 0);
        } else if (strcmp(name, "editing") == 0 || strcmp(name, "transaction") == 0) {
            memset(&f->preview, 0x5a, sizeof(f->preview)); memcpy(&f->preview_before, &f->preview, sizeof(f->preview));
            if (strcmp(name, "editing") == 0) {
                CHECK(umi_ui_workspace_customisation_begin_edit(&f->model) == UMI_STATUS_OK); f->before = f->model;
            } else CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
            CHECK(umi_ui_workspace_library_checkpoint_preview(server, &scope, &f->model, &f->preview) == UMI_STATUS_BUSY);
            CHECK(memcmp(&f->preview, &f->preview_before, sizeof(f->preview)) == 0);
            if (strcmp(name, "transaction") == 0) {
                CHECK(umi_data_server_in_transaction(server)); CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
            }
        } else return 2;
        CHECK(umi_data_server_count(server) == records);
        CHECK(memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *f = calloc(1U, sizeof(*f));
    UmiDataServer *server = NULL;
    CHECK(f != NULL && umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    int result = Run(f, server, argv[1]);
    umi_data_server_destroy(server); free(f);
    return result;
}
