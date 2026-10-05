/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_workspace_library_history.c
 * PURPOSE: Exercise transactional layout history against real canonical archives.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workspace_library_history.h"
#include "umicom/test_runtime/check.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const UmiUiWorkspaceCheckpointScope scope = {"org.umicom.fixture", "desktop", "org.umicom.fixture."};
static const char layout_id[] = "org.umicom.fixture.primary";
typedef struct HistoryFixture {
    UmiUiWorkspaceLibraryHistory *history;
    UmiUiWorkspaceCustomisation *model;
    UmiStatus failure;
    UmiStatus nested_status;
    size_t calls;
    bool try_nested;
    bool observed_busy;
} HistoryFixture;

/* A rejecting renderer never publishes its candidate. Reentrant navigation
 * observes BUSY before it can acquire another candidate or move the cursor. */
static UmiStatus publish(const UmiUiWorkspaceCustomisation *candidate, void *context)
{
    HistoryFixture *fixture = context;
    UmiUiWorkspaceLibraryHistoryState state;
    ++fixture->calls;
    if (umi_ui_workspace_library_history_read(fixture->history, fixture->model->revision, &state) == UMI_STATUS_OK)
        fixture->observed_busy = state.busy;
    if (fixture->try_nested)
        fixture->nested_status = umi_ui_workspace_library_history_navigate(fixture->history, &scope,
            fixture->model, UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO, fixture->model->revision, publish, fixture);
    if (fixture->failure != UMI_STATUS_OK) return fixture->failure;
    *fixture->model = *candidate;
    return UMI_STATUS_OK;
}

/* Rename through the public model contract so these tests exercise real
 * revision and UTF-8 rules, rather than constructing artificial archives. */
static UmiStatus rename_candidate(UmiUiWorkspaceCustomisation *candidate,
    const UmiUiWorkspaceCustomisation *model, const char *name)
{
    *candidate = *model;
    const UmiUiWorkspaceLibraryPolicy policy = {scope.layout_prefix};
    const UmiUiWorkspaceLibraryRequest request = {
        UMI_UI_WORKSPACE_LIBRARY_RENAME, layout_id, NULL, name, model->revision, false};
    return umi_ui_workspace_library_apply(candidate, &policy, &request, NULL);
}

int main(void)
{
    HistoryFixture fixture = {0};
    UmiUiWorkspaceCustomisation *model = malloc(sizeof(*model));
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate));
    UmiUiWorkspaceCustomisation *before = malloc(sizeof(*before));
    UmiUiWorkspaceLibraryHistoryState state;
    UMI_TEST_REQUIRE(model != NULL && candidate != NULL && before != NULL);
    fixture.model = model;
    umi_ui_workspace_customisation_init(model);
    UmiUiWindowDescriptor tool = {0};
    memcpy(tool.tool_id, "editor", sizeof("editor"));
    memcpy(tool.title, "Editor", sizeof("Editor"));
    tool.category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
    tool.default_width = 0.5; tool.default_height = 0.5;
    UMI_TEST_REQUIRE(umi_ui_window_catalogue_register(&model->windows, &tool) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(model, layout_id, "Original") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_open_window(model, "editor", "canvas", false, 1U,
        window_id, sizeof(window_id)) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_place_canvas_window(model, "editor", 0.125, 0.25, 0.5, 0.5) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_create(&fixture.history) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO, model->revision, publish, &fixture) == UMI_STATUS_NOT_FOUND);
    UMI_TEST_REQUIRE(rename_candidate(candidate, model, "Renamed") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(candidate) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_place_canvas_window(candidate, "editor", 0.25, 0.125, 0.5, 0.5) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(candidate) == UMI_STATUS_OK);
    *before = *model;
    fixture.failure = UMI_STATUS_UNAVAILABLE;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_record(fixture.history, &scope, model, candidate, publish, &fixture) == UMI_STATUS_UNAVAILABLE);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_read(fixture.history, model->revision, &state) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(state.undo_count == 0U && state.redo_count == 0U && !state.busy);
    fixture.failure = UMI_STATUS_OK; fixture.try_nested = true;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_record(fixture.history, &scope, model, candidate, publish, &fixture) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(fixture.observed_busy && fixture.nested_status == UMI_STATUS_BUSY);
    fixture.try_nested = false;
    /* Releasing or modifying the original candidate cannot alter recovery. */
    memset(candidate, 0xa5, sizeof(*candidate));
    uint64_t revision = model->revision;
    size_t calls = fixture.calls;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO, revision - 1U, publish, &fixture) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(fixture.calls == calls);
    UmiUiWorkspaceCheckpointScope foreign = scope; foreign.workspace_id = "another";
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &foreign, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO, revision, publish, &fixture) == UMI_STATUS_PERMISSION_DENIED);
    fixture.failure = UMI_STATUS_UNAVAILABLE; *before = *model;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO, revision, publish, &fixture) == UMI_STATUS_UNAVAILABLE);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    fixture.failure = UMI_STATUS_OK;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO, revision, publish, &fixture) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(strcmp(model->layouts[0].name, "Original") == 0 && model->revision > revision);
    UMI_TEST_REQUIRE(model->layouts[0].windows[0].x == 0.125 && model->layouts[0].windows[0].y == 0.25);
    UMI_TEST_REQUIRE(memcmp(&model->windows, &before->windows, sizeof(model->windows)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_read(fixture.history, model->revision, &state) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(state.undo_count == 0U && state.redo_count == 1U && !state.stale);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_REDO, model->revision, publish, &fixture) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(strcmp(model->layouts[0].name, "Renamed") == 0);
    UMI_TEST_REQUIRE(model->layouts[0].windows[0].x == 0.25 && model->layouts[0].windows[0].y == 0.125);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO, model->revision, publish, &fixture) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(rename_candidate(candidate, model, "Branch") == UMI_STATUS_OK);
    calls = fixture.calls;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_record(fixture.history, &foreign, model, candidate, publish, &fixture) == UMI_STATUS_PERMISSION_DENIED);
    UMI_TEST_REQUIRE(fixture.calls == calls);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_read(fixture.history, model->revision, &state) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(state.redo_count == 1U);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_record(fixture.history, &scope, model, candidate, publish, &fixture) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_REDO, model->revision, publish, &fixture) == UMI_STATUS_NOT_FOUND);

    /* An untracked edit invalidates navigation without erasing old evidence.
     * Only a successful later recorded change starts a fresh chain. */
    ++model->revision;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_read(fixture.history, model->revision, &state) == UMI_STATUS_OK && state.stale);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO, model->revision, publish, &fixture) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(rename_candidate(candidate, model, "Fresh") == UMI_STATUS_OK);
    fixture.failure = UMI_STATUS_UNAVAILABLE;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_record(fixture.history, &scope, model, candidate, publish, &fixture) == UMI_STATUS_UNAVAILABLE);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_read(fixture.history, model->revision, &state) == UMI_STATUS_OK && state.stale);
    fixture.failure = UMI_STATUS_OK;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_record(fixture.history, &scope, model, candidate, publish, &fixture) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_read(fixture.history, model->revision, &state) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(state.undo_count == 1U && state.redo_count == 0U && !state.stale);
    for (size_t index = 0U; index < UMI_UI_WORKSPACE_LIBRARY_HISTORY_LIMIT + 3U; ++index) {
        char name[64]; (void)snprintf(name, sizeof(name), "Change %zu", index);
        UMI_TEST_REQUIRE(rename_candidate(candidate, model, name) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_ui_workspace_library_history_record(fixture.history, &scope, model, candidate, publish, &fixture) == UMI_STATUS_OK);
    }
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_read(fixture.history, model->revision, &state) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(state.undo_count == UMI_UI_WORKSPACE_LIBRARY_HISTORY_LIMIT);
    for (size_t index = 0U; index < UMI_UI_WORKSPACE_LIBRARY_HISTORY_LIMIT; ++index)
        UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
            UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO, model->revision, publish, &fixture) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(strcmp(model->layouts[0].name, "Change 2") == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO, model->revision, publish, &fixture) == UMI_STATUS_NOT_FOUND);
    /* Invalid direction, active edits and malformed scope never publish. */
    calls = fixture.calls;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        (UmiUiWorkspaceLibraryHistoryDirection)99, model->revision, publish, &fixture) == UMI_STATUS_INVALID_ARGUMENT);
    model->edit_active = true;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &scope, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_REDO, model->revision, publish, &fixture) == UMI_STATUS_BUSY);
    model->edit_active = false;
    foreign.application_id = NULL;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_history_navigate(fixture.history, &foreign, model,
        UMI_UI_WORKSPACE_LIBRARY_HISTORY_REDO, model->revision, publish, &fixture) == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_REQUIRE(fixture.calls == calls);
    umi_ui_workspace_library_history_destroy(fixture.history);
    free(before); free(candidate); free(model);
    return EXIT_SUCCESS;
}
