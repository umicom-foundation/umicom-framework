/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_library_order.c
 * PURPOSE: Verify ordered layout moves, revision rejection and archive recovery.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_library.h"
#include "umicom/ui/workspace_library_checkpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef struct Fixture {
    UmiUiWorkspaceCustomisation model, before, restored;
    UmiUiWorkspaceLibrarySnapshot rows, previous_rows;
} Fixture;
static const UmiUiWorkspaceLibraryPolicy policy = {"test.order."};
static const UmiUiWorkspaceCheckpointScope scope = {"org.umicom.order-test", "test", "test.order."};

/* Seed three distinct, committed layouts with a real registered panel. */
static int Seed(Fixture *f)
{
    UmiUiWindowDescriptor tool = {0};
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    umi_ui_workspace_customisation_init(&f->model);
    strcpy(tool.tool_id, "editor"); strcpy(tool.title, "Editor");
    tool.category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
    tool.default_width = 0.4; tool.default_height = 0.5;
    CHECK(umi_ui_window_catalogue_register(&f->model.windows, &tool) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(&f->model, "test.order.a", "First") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_begin_edit(&f->model) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_open_window(&f->model, "editor", "canvas", false, 1U, window_id, sizeof(window_id)) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_commit_edit(&f->model) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(&f->model, "test.order.b", "Second") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(&f->model, "test.order.c", "Third") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_activate(&f->model, "test.order.a") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_library_snapshot(&f->model, &policy, &f->rows) == UMI_STATUS_OK);
    f->before = f->model; f->previous_rows = f->rows;
    return 0;
}

/* Check both observable outputs stay untouched when the owner refuses a move. */
static int Reject(Fixture *f, const UmiUiWorkspaceLibraryPolicy *owner,
    const UmiUiWorkspaceLibraryRequest *request, UmiStatus expected)
{
    f->before = f->model; f->previous_rows = f->rows;
    CHECK(umi_ui_workspace_library_apply(&f->model, owner, request, &f->rows) == expected);
    CHECK(memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
    CHECK(memcmp(&f->rows, &f->previous_rows, sizeof(f->rows)) == 0);
    return 0;
}

/* Save, change the order, and read back the same archive through real storage. */
static int Archive(Fixture *f, UmiDataServer *server)
{
    UmiUiWorkspaceLibraryCheckpointReport report;
    UmiUiWorkspaceLibraryRequest move = {UMI_UI_WORKSPACE_LIBRARY_MOVE_EARLIER,
        "test.order.c", NULL, NULL, f->model.revision, false};
    CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &move, NULL) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_library_checkpoint_save(server, &scope, &f->model, 123U, 0U, &report) == UMI_STATUS_OK);
    move.action = UMI_UI_WORKSPACE_LIBRARY_MOVE_LATER;
    move.expected_customisation_revision = f->model.revision;
    CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &move, NULL) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_library_checkpoint_load_candidate(server, &scope, &f->model, &f->restored, &report) == UMI_STATUS_OK);
    CHECK(strcmp(f->restored.layouts[1].layout_id, "test.order.c") == 0);
    CHECK(strcmp(f->restored.layouts[2].layout_id, "test.order.b") == 0);
    CHECK(strcmp(f->restored.active_layout_id, "test.order.a") == 0);
    CHECK(f->restored.layouts[0].window_count == f->model.layouts[0].window_count);
    CHECK(f->restored.layouts[0].windows[0].x == f->model.layouts[0].windows[0].x);
    CHECK(!report.checkpoint.durable && report.layout_count == 3U);
    return 0;
}

static int Run(Fixture *f, const char *name, UmiDataServer *server)
{
    CHECK(Seed(f) == 0);
    UmiUiWorkspaceLibraryRequest move = {UMI_UI_WORKSPACE_LIBRARY_MOVE_EARLIER,
        "test.order.b", NULL, NULL, f->model.revision, false};
    if (strcmp(name, "move") == 0 || strcmp(name, "alias") == 0) {
        if (strcmp(name, "alias") == 0) move.target_layout_id = f->model.layouts[1].layout_id;
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &move, &f->rows) == UMI_STATUS_OK);
        CHECK(f->model.revision == f->before.revision + 1U);
        CHECK(memcmp(&f->model.layouts[0], &f->before.layouts[1], sizeof(f->model.layouts[0])) == 0);
        CHECK(memcmp(&f->model.layouts[1], &f->before.layouts[0], sizeof(f->model.layouts[0])) == 0);
        CHECK(memcmp(&f->model.layouts[2], &f->before.layouts[2], sizeof(f->model.layouts[0])) == 0);
        CHECK(strcmp(f->model.active_layout_id, f->before.active_layout_id) == 0);
        CHECK(memcmp(&f->model.windows, &f->before.windows, sizeof(f->model.windows)) == 0);
        CHECK(memcmp(&f->model.groups, &f->before.groups, sizeof(f->model.groups)) == 0);
        CHECK(memcmp(&f->model.library, &f->before.library, sizeof(f->model.library)) == 0);
        CHECK(memcmp(&f->model.theme, &f->before.theme, sizeof(f->model.theme)) == 0);
        move.target_layout_id = "test.order.b"; move.action = UMI_UI_WORKSPACE_LIBRARY_MOVE_LATER;
        move.expected_customisation_revision = f->model.revision;
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &move, NULL) == UMI_STATUS_OK);
        CHECK(memcmp(f->model.layouts, f->before.layouts, sizeof(f->model.layouts)) == 0);
    } else if (strcmp(name, "boundaries") == 0) {
        move.target_layout_id = "test.order.a";
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &move, &f->rows) == UMI_STATUS_OK);
        CHECK(memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
        move.target_layout_id = "test.order.c"; move.action = UMI_UI_WORKSPACE_LIBRARY_MOVE_LATER;
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &move, NULL) == UMI_STATUS_OK);
        CHECK(memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
    } else if (strcmp(name, "stale") == 0) {
        --move.expected_customisation_revision;
        CHECK(Reject(f, &policy, &move, UMI_STATUS_INVALID_STATE) == 0);
    } else if (strcmp(name, "editing") == 0) {
        CHECK(umi_ui_workspace_customisation_begin_edit(&f->model) == UMI_STATUS_OK);
        move.expected_customisation_revision = f->model.revision;
        CHECK(Reject(f, &policy, &move, UMI_STATUS_BUSY) == 0);
    } else if (strcmp(name, "scope") == 0) {
        const UmiUiWorkspaceLibraryPolicy foreign = {"other."};
        CHECK(Reject(f, &foreign, &move, UMI_STATUS_PERMISSION_DENIED) == 0);
        move.target_layout_id = "other.b";
        CHECK(Reject(f, &policy, &move, UMI_STATUS_PERMISSION_DENIED) == 0);
    } else if (strcmp(name, "invalid") == 0) {
        move.target_layout_id = "test.order.missing";
        CHECK(Reject(f, &policy, &move, UMI_STATUS_NOT_FOUND) == 0);
        move.target_layout_id = NULL;
        CHECK(Reject(f, &policy, &move, UMI_STATUS_INVALID_ARGUMENT) == 0);
        move.action = (UmiUiWorkspaceLibraryAction)7;
        CHECK(Reject(f, &policy, &move, UMI_STATUS_INVALID_ARGUMENT) == 0);
        move.action = UMI_UI_WORKSPACE_LIBRARY_MOVE_LATER; move.target_layout_id = "test.order.b";
        memset(f->model.layouts[1].name, 'x', sizeof(f->model.layouts[1].name));
        CHECK(Reject(f, &policy, &move, UMI_STATUS_INVALID_STATE) == 0);
    } else if (strcmp(name, "overflow") == 0) {
        f->model.revision = UINT64_MAX; move.expected_customisation_revision = UINT64_MAX;
        CHECK(Reject(f, &policy, &move, UMI_STATUS_CAPACITY_EXCEEDED) == 0);
        move.target_layout_id = "test.order.a";
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &move, NULL) == UMI_STATUS_OK);
        CHECK(memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
    } else if (strcmp(name, "checkpoint") == 0) return Archive(f, server);
    else return 2;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    UmiDataServer *server = NULL;
    if (fixture == NULL) return 1;
    if (umi_data_server_create_memory(&server) != UMI_STATUS_OK) { free(fixture); return 1; }
    int result = Run(fixture, argv[1], server);
    umi_data_server_destroy(server); free(fixture);
    return result;
}
