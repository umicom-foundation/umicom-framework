/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_workspace_zen.c
 *
 * PURPOSE:
 *   Check reversible Zen identities, refusal atomicity and unchanged layout contents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/workspace_zen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); failed = 1; goto cleanup; } } while (0)
#define ID(name) UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX name

/* Simulate a restored library which no longer contains a remembered target.
 * Only the library authority changes; the transition must not invent a layout. */
static void RemoveFixtureLayout(UmiUiWorkspaceCustomisation *model, const char *id)
{
    for (size_t i = 0U; i < model->layout_count; ++i) {
        if (strcmp(model->layouts[i].layout_id, id) != 0) continue;
        memmove(&model->layouts[i], &model->layouts[i + 1U],
            (model->layout_count - i - 1U) * sizeof model->layouts[0]);
        --model->layout_count;
        memset(&model->layouts[model->layout_count], 0, sizeof model->layouts[0]);
        return;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    const char *modes[] = {"roundtrip", "direct-zen", "missing-return", "intervening-selection",
        "busy-enter", "busy-exit", "missing-zen", "missing-default", "revision-limit",
        "bad-active", "bad-return", "bad-record", "duplicate", "empty", "count", "null"};
    bool known = false;
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(mode, modes[i]) == 0) known = true;
    if (!known) return 2;
    int failed = 0;
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof *model);
    UmiUiWorkspaceCustomisation *before = malloc(sizeof *before);
    UmiStudioWorkspaceZenState state = {0}, saved;
    CHECK(model != NULL && before != NULL);
    umi_ui_workspace_customisation_init(model);
    CHECK(umi_studio_runtime_workspace_seed(model) == UMI_STATUS_OK);
    CHECK(umi_studio_runtime_workspace_select(model, "debugging") == UMI_STATUS_OK);
    if (strcmp(mode, "null") == 0) {
        *before = *model;
        CHECK(umi_studio_runtime_workspace_toggle_zen(NULL, &state) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_studio_runtime_workspace_toggle_zen(model, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_studio_runtime_workspace_zen_can_toggle(NULL, &state));
        CHECK(!umi_studio_runtime_workspace_zen_can_toggle(model, NULL));
        CHECK(memcmp(model, before, sizeof *model) == 0 && state.return_layout_id[0] == '\0');
        goto cleanup;
    }
    bool direct = strcmp(mode, "direct-zen") == 0 || strcmp(mode, "missing-return") == 0 ||
        strcmp(mode, "missing-default") == 0;
    if (direct) CHECK(umi_studio_runtime_workspace_select(model, "zen") == UMI_STATUS_OK);
    if (strcmp(mode, "missing-return") == 0) strcpy(state.return_layout_id, ID("removed"));
    if (strcmp(mode, "intervening-selection") == 0) {
        CHECK(umi_studio_runtime_workspace_toggle_zen(model, &state) == UMI_STATUS_OK);
        CHECK(umi_studio_runtime_workspace_select(model, "testing") == UMI_STATUS_OK);
    }
    if (strcmp(mode, "busy-exit") == 0)
        CHECK(umi_studio_runtime_workspace_toggle_zen(model, &state) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_OK;
    if (strncmp(mode, "busy-", 5U) == 0) {
        CHECK(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
        expected = UMI_STATUS_BUSY;
    } else if (strcmp(mode, "missing-zen") == 0) {
        RemoveFixtureLayout(model, ID("zen")); expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "missing-default") == 0) {
        RemoveFixtureLayout(model, ID("default")); expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "revision-limit") == 0) {
        model->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "bad-active") == 0) {
        memset(model->active_layout_id, 'x', sizeof model->active_layout_id); expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "bad-return") == 0) {
        memset(state.return_layout_id, 'x', sizeof state.return_layout_id); expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "bad-record") == 0) {
        memset(model->layouts[0].layout_id, 'x', sizeof model->layouts[0].layout_id); expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "duplicate") == 0) {
        memcpy(model->layouts[1].layout_id, model->layouts[0].layout_id, sizeof model->layouts[0].layout_id);
        expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "empty") == 0) {
        model->layout_count = 0U; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "count") == 0) {
        model->layout_count = UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS + 1U; expected = UMI_STATUS_INVALID_STATE;
    }
    *before = *model; saved = state;
    CHECK(umi_studio_runtime_workspace_zen_can_toggle(model, &state) == (expected == UMI_STATUS_OK));
    CHECK(memcmp(model, before, sizeof *model) == 0 && memcmp(&state, &saved, sizeof state) == 0);
    CHECK(umi_studio_runtime_workspace_toggle_zen(model, &state) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(memcmp(model, before, sizeof *model) == 0 && memcmp(&state, &saved, sizeof state) == 0);
    } else {
        CHECK(memcmp(model->layouts, before->layouts, sizeof model->layouts) == 0);
        CHECK(memcmp(&model->groups, &before->groups, sizeof model->groups) == 0);
        CHECK(memcmp(&model->library, &before->library, sizeof model->library) == 0);
        CHECK(model->revision == before->revision + 1U);
        if (direct) {
            CHECK(strcmp(model->active_layout_id, ID("default")) == 0);
            CHECK(state.return_layout_id[0] == '\0');
        } else {
            CHECK(strcmp(model->active_layout_id, ID("zen")) == 0);
            CHECK(strcmp(state.return_layout_id, before->active_layout_id) == 0);
            CHECK(umi_studio_runtime_workspace_toggle_zen(model, &state) == UMI_STATUS_OK);
            CHECK(strcmp(model->active_layout_id, before->active_layout_id) == 0);
            CHECK(state.return_layout_id[0] == '\0');
            before->revision += 2U;
            CHECK(memcmp(model, before, sizeof *model) == 0);
        }
    }
cleanup:
    free(before); free(model);
    return failed;
}
