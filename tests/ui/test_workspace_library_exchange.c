/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_workspace_library_exchange.c
 * PURPOSE: Exercise immutable import evidence and atomic refusal at the public archive boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_library_exchange.h"
#include "umicom/test_runtime/check.h"
#include <stdlib.h>
#include <string.h>
static const UmiUiWorkspaceCheckpointScope fixture_scope = {"org.umicom.fixture", "desktop", "org.umicom.fixture."};
static const char alpha_id[] = "org.umicom.fixture.alpha";
static const char beta_id[] = "org.umicom.fixture.beta";
static const char empty_id[] = "org.umicom.fixture.empty";
/* Build independent named layouts using public C contracts. Context members
 * include a non-layout observer which a restore must never remove. */
static int seed_model(UmiUiWorkspaceCustomisation *model)
{
    UmiUiWindowDescriptor tool = {0};
    UmiUiWorkspaceLayout *active;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    umi_ui_workspace_customisation_init(model);
    memcpy(tool.tool_id, "editor", sizeof("editor"));
    memcpy(tool.title, "Editor", sizeof("Editor"));
    tool.category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
    tool.default_width = 0.40;
    tool.default_height = 0.50;
    UMI_TEST_REQUIRE(umi_ui_window_catalogue_register(&model->windows, &tool) == UMI_STATUS_OK);
    memcpy(tool.tool_id, "inspector", sizeof("inspector"));
    memcpy(tool.title, "Inspector", sizeof("Inspector"));
    UMI_TEST_REQUIRE(umi_ui_window_catalogue_register(&model->windows, &tool) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_window_group_define(&model->groups, "fixture.context", "green",
        UMI_UI_WINDOW_CONTEXT_FILE) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_window_group_assign(&model->groups, "fixture.context", "outside.observer",
        UMI_UI_WINDOW_GROUP_DESTINATION) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(model, alpha_id, "First canvas") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_open_window(model, "editor", "canvas", false,
        1U, window_id, sizeof(window_id)) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_place_canvas_window(model, "editor",
        0.125, 0.25, 0.50, 0.50) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_clone_layout(model, alpha_id, beta_id, "Second canvas") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_activate(model, beta_id) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_open_window(model, "inspector", "left", false,
        2U, window_id, sizeof(window_id)) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_set_auto_hidden(model, "inspector", true) == UMI_STATUS_OK);
    active = umi_ui_workspace_customisation_active(model);
    UMI_TEST_REQUIRE(active != NULL && active->window_count == 2U);
    memcpy(active->windows[0].context_group_id, "fixture.context", sizeof("fixture.context"));
    UMI_TEST_REQUIRE(umi_ui_window_group_assign(&model->groups, "fixture.context", "editor",
        UMI_UI_WINDOW_GROUP_SOURCE) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(model, empty_id, "Empty canvas") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_activate(model, beta_id) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}


/* Use real windows, geometry, hidden panels and a context observer. A review
 * must freeze content without capturing or replacing the owner's catalogues. */
int main(void)
{
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *before = malloc(sizeof(*before));
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate));
    UmiUiWorkspaceLibraryImport *review = NULL;
    UMI_TEST_REQUIRE(model != NULL && before != NULL && candidate != NULL);
    UMI_TEST_REQUIRE(seed_model(model) == EXIT_SUCCESS);
    *before = *model; memset(candidate, 0x3a, sizeof(*candidate));
    size_t size = 0U, required = 0U;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_export(&fixture_scope, model, 42U, NULL, 0U, &size) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(size > 1U && size < UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY);
    char *archive = malloc(size + 2U);
    UMI_TEST_REQUIRE(archive != NULL); memset(archive, 'x', size + 2U);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_export(&fixture_scope, model, 42U, archive, size, &required) == UMI_STATUS_CAPACITY_EXCEEDED);
    UMI_TEST_REQUIRE(required == size && archive[0] == 'x' && archive[size - 1U] == 'x');
    UMI_TEST_REQUIRE(umi_ui_workspace_library_export(&fixture_scope, model, 42U, archive, size + 1U, &required) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(required == size && archive[size] == '\0');
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_review(&fixture_scope, model, archive, size, &review) == UMI_STATUS_OK);
    const UmiUiWorkspaceLibraryPreview *summary = umi_ui_workspace_library_import_summary(review);
    UMI_TEST_REQUIRE(summary != NULL && summary->saved.layout_count == 3U && summary->comparison.current_revision == model->revision);
    UMI_TEST_REQUIRE(summary->report.checkpoint.saved_at_ns == 42U && !summary->report.checkpoint.durable);
    /* Full layout evidence includes inactive layouts, hidden panels, geometry
     * and linked context, not merely the summary's panel count. */
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_layout_count(review, false) == 3U);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_layout_count(review, true) == 3U);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_layout_count(NULL, false) == 0U);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_layout(NULL, true, 0U) == NULL);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_layout(review, false, SIZE_MAX) == NULL);
    const UmiUiWorkspaceLayout *captured = umi_ui_workspace_library_import_layout(review, false, 1U);
    const UmiUiWorkspaceLayout *imported = umi_ui_workspace_library_import_layout(review, true, 1U);
    UMI_TEST_REQUIRE(captured && imported && captured != &model->layouts[1]);
    UMI_TEST_REQUIRE(captured->window_count == 2U && imported->window_count == 2U);
    UMI_TEST_REQUIRE(imported->windows[0].x == 0.125 && imported->windows[0].width == 0.50);
    UMI_TEST_REQUIRE(!strcmp(imported->windows[0].context_group_id, "fixture.context"));
    UMI_TEST_REQUIRE(!imported->windows[1].visible);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_layout(review, true, 2U)->window_count == 0U);
    /* Changing or releasing the source bytes cannot change an existing review. */
    archive[0] = '!';
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_candidate(review, &fixture_scope, model, candidate) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(candidate->revision == model->revision + 1U && candidate->layout_count == 3U);
    UMI_TEST_REQUIRE(strcmp(candidate->active_layout_id, beta_id) == 0);
    UMI_TEST_REQUIRE(candidate->layouts[1].windows[0].x == 0.125);
    UMI_TEST_REQUIRE(memcmp(&candidate->windows, &model->windows, sizeof(model->windows)) == 0);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    UmiUiWorkspaceCustomisation *saved_output = malloc(sizeof(*saved_output));
    UMI_TEST_REQUIRE(saved_output != NULL); *saved_output = *candidate;
    /* Editing the live model cannot mutate either borrowed evidence row. */
    model->layouts[1].windows[0].x = 0.25;
    UMI_TEST_REQUIRE(captured->windows[0].x == 0.125 && imported->windows[0].x == 0.125);
    ++model->revision;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_candidate(review, &fixture_scope, model, candidate) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(memcmp(candidate, saved_output, sizeof(*candidate)) == 0);
    *model = *before;
    UmiUiWorkspaceCheckpointScope foreign = fixture_scope; foreign.workspace_id = "another";
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_candidate(review, &foreign, model, candidate) == UMI_STATUS_PERMISSION_DENIED);
    UMI_TEST_REQUIRE(memcmp(candidate, saved_output, sizeof(*candidate)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_candidate(review, &fixture_scope, model, model) == UMI_STATUS_INVALID_ARGUMENT);
    model->edit_active = true;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_candidate(review, &fixture_scope, model, candidate) == UMI_STATUS_BUSY);
    *model = *before;
    /* A malformed replacement must preserve the caller's existing review. */
    UmiUiWorkspaceLibraryImport *same_review = review;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_review(&fixture_scope, model, archive, size, &review) != UMI_STATUS_OK);
    UMI_TEST_REQUIRE(review == same_review);
    archive[0] = 'U';
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_review(&fixture_scope, model, archive, size + 1U, &review) == UMI_STATUS_PARSE_ERROR);
    UMI_TEST_REQUIRE(review == same_review);
    for (size_t cut = 0U; cut < size; cut += size / 11U + 1U) {
        UMI_TEST_REQUIRE(umi_ui_workspace_library_import_review(&fixture_scope, model, archive, cut, &review) != UMI_STATUS_OK);
        UMI_TEST_REQUIRE(review == same_review);
    }
    archive[size] = '!';
    UMI_TEST_REQUIRE(umi_ui_workspace_library_import_review(&fixture_scope, model, archive, size + 1U, &review) != UMI_STATUS_OK);
    UMI_TEST_REQUIRE(review == same_review && memcmp(model, before, sizeof(*model)) == 0);
    umi_ui_workspace_library_import_destroy(review);
    free(saved_output); free(archive); free(candidate); free(before); free(model);
    return EXIT_SUCCESS;
}
