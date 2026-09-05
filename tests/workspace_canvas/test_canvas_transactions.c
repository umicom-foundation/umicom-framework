/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workspace_canvas/test_canvas_transactions.c
 *
 * PURPOSE:
 *   Execute the portable blank-layout, clear-canvas and free-placement contracts
 *   against the real Framework layout and context-group implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_customisation.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Keep checks active in Release builds; assert would disappear with NDEBUG. */
#define CHECK(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

#ifdef UMICOM_TEST_WRAP_MALLOC
static bool failAllocation;
void *__real_malloc(size_t size);
void *__wrap_malloc(size_t size);
void *__wrap_malloc(size_t size)
{
    return failAllocation ? NULL : __real_malloc(size);
}
#endif

/* These are layout-only fixtures. They intentionally do not start application,
 * theme, catalogue, database or GUI services; those are separate acceptance
 * layers. All production functions exercised here are linked from real source. */
static UmiUiWorkspaceCustomisation *makeWorkspace(void)
{
    UmiUiWorkspaceCustomisation *workspace = calloc(1U, sizeof(*workspace));
    CHECK(workspace != NULL);
    workspace->revision = 1U;
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "work", "Work") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_begin_edit(workspace) == UMI_STATUS_OK);
    return workspace;
}

static UmiUiWorkspaceCustomisation *copyWorkspace(
    const UmiUiWorkspaceCustomisation *workspace)
{
    UmiUiWorkspaceCustomisation *copy = calloc(1U, sizeof(*copy));
    CHECK(copy != NULL);
    (void)memcpy(copy, workspace, sizeof(*copy));
    return copy;
}

static void addWindow(UmiUiWorkspaceCustomisation *workspace,
                      const char *id, bool closable, bool pinned)
{
    UmiUiWorkspaceWindow window = {0};
    CHECK(strlen(id) < sizeof(window.window_id));
    (void)snprintf(window.window_id, sizeof(window.window_id), "%s", id);
    (void)snprintf(window.tool_id, sizeof(window.tool_id), "%s", "test.tool");
    (void)snprintf(window.title, sizeof(window.title), "%s", "Test surface");
    (void)snprintf(window.placement_id, sizeof(window.placement_id), "%s", "left");
    (void)snprintf(window.group_id, sizeof(window.group_id), "%s", "left");
    (void)snprintf(window.stack_id, sizeof(window.stack_id), "%s", "left");
    window.width = 0.25;
    window.height = 0.5;
    window.visible = true;
    window.closable = closable;
    window.pinned = pinned;
    window.resizable = true;
    CHECK(umi_ui_workspace_layout_add_window(
        umi_ui_workspace_customisation_active(workspace), &window) == UMI_STATUS_OK);
}

static void testBlankCreation(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceLayout previous;
    addWindow(workspace, "editor", true, false);
    CHECK(umi_ui_workspace_customisation_commit_edit(workspace) == UMI_STATUS_OK);
    previous = workspace->layouts[0];
    workspace->windows.count = 1U; /* Catalogue preservation sentinel. */
    (void)snprintf(workspace->windows.items[0].tool_id,
        sizeof(workspace->windows.items[0].tool_id), "%s", "editor.type");
    workspace->theme.font_scale = 1.5;
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "blank", workspace->layouts[0].name) == UMI_STATUS_OK);
    CHECK(workspace->layout_count == 2U);
    CHECK(strcmp(workspace->active_layout_id, "blank") == 0);
    CHECK(umi_ui_workspace_customisation_active(workspace)->locked);
    CHECK(umi_ui_workspace_customisation_active(workspace)->window_count == 0U);
    CHECK(!workspace->edit_active);
    CHECK(memcmp(&previous, &workspace->layouts[0], sizeof(previous)) == 0);
    CHECK(workspace->windows.count == 1U);
    CHECK(strcmp(workspace->windows.items[0].tool_id, "editor.type") == 0);
    CHECK(workspace->theme.font_scale == 1.5);
    free(workspace);
}

static void testBlankFailureAtomicity(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceCustomisation *before = copyWorkspace(workspace);
    char overlong[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY + 2U];
    (void)memset(overlong, 'x', sizeof(overlong));
    overlong[sizeof(overlong) - 1U] = '\0';
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "new", "New") == UMI_STATUS_BUSY);
    CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    CHECK(umi_ui_workspace_customisation_cancel_edit(workspace) == UMI_STATUS_OK);
    (void)memcpy(before, workspace, sizeof(*before));
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "work", "Duplicate") == UMI_STATUS_ALREADY_EXISTS);
    CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "", "Empty ID") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "new", "") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, overlong, "Long ID") == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "new", overlong) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        NULL, "new", "New") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, NULL, "New") == UMI_STATUS_INVALID_ARGUMENT);
    free(before);
    free(workspace);
}

static void testBlankCapacity(void)
{
    UmiUiWorkspaceCustomisation *workspace = calloc(1U, sizeof(*workspace));
    UmiUiWorkspaceCustomisation *before;
    size_t index;
    CHECK(workspace != NULL);
    for (index = 0U; index < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS; ++index) {
        char id[32U];
        (void)snprintf(id, sizeof(id), "layout-%zu", index);
        CHECK(umi_ui_workspace_customisation_create_blank_layout(
            workspace, id, "Layout") == UMI_STATUS_OK);
    }
    before = copyWorkspace(workspace);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "overflow", "Overflow") == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    free(before);
    free(workspace);
}

static void testClearCancelAndCommit(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceCustomisation *before;
    UmiUiWorkspaceCanvasClearResult result = {99U, 99U};
    addWindow(workspace, "chart", true, false);
    addWindow(workspace, "quotes", true, false);
    CHECK(umi_ui_window_group_define(&workspace->groups, "instrument",
        "blue", UMI_UI_WINDOW_CONTEXT_INSTRUMENT) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_assign_context_group(workspace,
        "chart", "instrument", UMI_UI_WINDOW_GROUP_SOURCE) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_assign_context_group(workspace,
        "quotes", "instrument", UMI_UI_WINDOW_GROUP_DESTINATION) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_commit_edit(workspace) == UMI_STATUS_OK);
    before = copyWorkspace(workspace);
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, &result) ==
        UMI_STATUS_INVALID_STATE);
    CHECK(result.removed == 99U && result.retained == 99U);
    CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    CHECK(umi_ui_workspace_customisation_begin_edit(workspace) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, &result) == UMI_STATUS_OK);
    CHECK(result.removed == 2U && result.retained == 0U);
    CHECK(umi_ui_workspace_customisation_active(workspace)->window_count == 0U);
    CHECK(workspace->groups.items[0].member_count == 0U);
    CHECK(workspace->groups.count == 1U);
    CHECK(workspace->edit_active);
    CHECK(umi_ui_workspace_customisation_cancel_edit(workspace) == UMI_STATUS_OK);
    CHECK(memcmp(&before->layouts, &workspace->layouts, sizeof(workspace->layouts)) == 0);
    CHECK(memcmp(&before->groups, &workspace->groups, sizeof(workspace->groups)) == 0);
    CHECK(umi_ui_workspace_customisation_begin_edit(workspace) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, NULL) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_commit_edit(workspace) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_active(workspace)->window_count == 0U);
    CHECK(umi_ui_workspace_customisation_active(workspace)->locked);
    free(before);
    free(workspace);
}

static void testProtectedAndDetached(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceCanvasClearResult result;
    UmiUiWorkspaceLayout *layout;
    uint64_t revision;
    addWindow(workspace, "removable", true, false);
    addWindow(workspace, "pinned", true, true);
    addWindow(workspace, "essential", false, false);
    addWindow(workspace, "detached", true, false);
    layout = umi_ui_workspace_customisation_active(workspace);
    layout->windows[3].floating = true;
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, &result) == UMI_STATUS_OK);
    layout = umi_ui_workspace_customisation_active(workspace);
    CHECK(result.removed == 2U && result.retained == 2U);
    CHECK(strcmp(layout->windows[0].window_id, "pinned") == 0);
    CHECK(strcmp(layout->windows[1].window_id, "essential") == 0);
    CHECK(layout->windows[2].window_id[0] == '\0');
    revision = workspace->revision;
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, &result) == UMI_STATUS_OK);
    CHECK(result.removed == 0U && result.retained == 2U);
    CHECK(workspace->revision == revision);
    free(workspace);
}

static void testSharedContextReference(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    addWindow(workspace, "shared", true, false);
    CHECK(umi_ui_window_group_define(&workspace->groups, "context",
        "blue", UMI_UI_WINDOW_CONTEXT_DOCUMENT) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_assign_context_group(workspace,
        "shared", "context", UMI_UI_WINDOW_GROUP_BIDIRECTIONAL) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_commit_edit(workspace) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_clone_layout(
        workspace, "work", "other", "Other") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_begin_edit(workspace) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, NULL) == UMI_STATUS_OK);
    CHECK(workspace->layouts[0].window_count == 1U);
    CHECK(workspace->layouts[1].window_count == 0U);
    CHECK(workspace->groups.items[0].member_count == 1U);
    CHECK(umi_ui_workspace_customisation_commit_edit(workspace) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_activate(workspace, "work") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_begin_edit(workspace) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, NULL) == UMI_STATUS_OK);
    CHECK(workspace->groups.items[0].member_count == 0U);
    free(workspace);
}

static void testCanvasPlacement(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceWindow *window;
    UmiUiWorkspaceLayout before;
    addWindow(workspace, "editor", true, false);
    window = &umi_ui_workspace_customisation_active(workspace)->windows[0];
    (void)snprintf(window->context_group_id, sizeof(window->context_group_id), "%s", "project");
    window->floating = true;
    window->maximised = true;
    CHECK(umi_ui_workspace_customisation_commit_edit(workspace) == UMI_STATUS_OK);
    before = *umi_ui_workspace_customisation_active(workspace);
    CHECK(umi_ui_workspace_customisation_begin_edit(workspace) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_place_canvas_window(workspace,
        window->window_id, 0.25, 0.125, 0.5, 0.5) == UMI_STATUS_OK);
    window = &umi_ui_workspace_customisation_active(workspace)->windows[0];
    CHECK(strcmp(window->placement_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT) == 0);
    CHECK(strcmp(window->stack_id, "editor") == 0);
    CHECK(strcmp(window->context_group_id, "project") == 0);
    CHECK(!window->floating && !window->maximised && window->visible);
    CHECK(window->x == 0.25 && window->width == 0.5);
    CHECK(umi_ui_workspace_customisation_place_canvas_window(workspace,
        "editor", 0.0, 0.0, 1.0, 1.0) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_cancel_edit(workspace) == UMI_STATUS_OK);
    CHECK(memcmp(&before, umi_ui_workspace_customisation_active(workspace), sizeof(before)) == 0);
    free(workspace);
}

static void testInvalidPlacement(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceCustomisation *before;
    const double bad[][4] = {
        {-0.1, 0.0, 0.25, 0.5}, {0.0, -0.1, 0.25, 0.5},
        {0.0, 0.0, 0.0, 0.5}, {0.0, 0.0, 0.25, -0.1},
        {0.9, 0.0, 0.25, 0.5}, {0.0, 0.75, 0.25, 0.5},
        {NAN, 0.0, 0.25, 0.5}, {0.0, INFINITY, 0.25, 0.5},
        {0.0, 0.0, NAN, 0.5}, {0.0, 0.0, 0.25, INFINITY}
    };
    size_t index;
    addWindow(workspace, "editor", true, false);
    before = copyWorkspace(workspace);
    for (index = 0U; index < sizeof(bad) / sizeof(bad[0]); ++index) {
        CHECK(umi_ui_workspace_customisation_place_canvas_window(workspace,
            "editor", bad[index][0], bad[index][1], bad[index][2], bad[index][3]) ==
            UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    }
    CHECK(umi_ui_workspace_customisation_place_canvas_window(workspace,
        "missing", 0.0, 0.0, 0.25, 0.5) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    free(before);
    free(workspace);
}

static void testPlacementPolicy(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceWindow *window;
    addWindow(workspace, "fixed", true, true);
    CHECK(umi_ui_workspace_customisation_place_canvas_window(workspace,
        "fixed", 0.25, 0.0, 0.25, 0.5) == UMI_STATUS_PERMISSION_DENIED);
    window = &umi_ui_workspace_customisation_active(workspace)->windows[0];
    window->pinned = false;
    window->resizable = false;
    CHECK(umi_ui_workspace_customisation_place_canvas_window(workspace,
        "fixed", 0.25, 0.0, 0.5, 0.5) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(umi_ui_workspace_customisation_place_canvas_window(workspace,
        "fixed", 0.25, 0.0, 0.25, 0.5) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_commit_edit(workspace) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_customisation_place_canvas_window(workspace,
        "fixed", 0.0, 0.0, 0.25, 0.5) == UMI_STATUS_INVALID_STATE);
    free(workspace);
}

static void testMalformedRecords(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceCustomisation *before;
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceCanvasClearResult result = {99U, 99U};
    addWindow(workspace, "editor", true, false);
    layout = umi_ui_workspace_customisation_active(workspace);
    layout->window_count = UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS + 1U;
    before = copyWorkspace(workspace);
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, &result) == UMI_STATUS_INVALID_STATE);
    CHECK(result.removed == 99U && result.retained == 99U);
    CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    layout->window_count = 1U;
    workspace->groups.count = UMI_UI_WINDOW_GROUP_MAX + 1U;
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, NULL) == UMI_STATUS_INVALID_STATE);
    workspace->groups.count = 1U;
    workspace->groups.items[0].member_count = UMI_UI_WINDOW_GROUP_MAX_MEMBERS + 1U;
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, NULL) == UMI_STATUS_INVALID_STATE);
    workspace->groups.items[0].member_count = 2U;
    (void)snprintf(workspace->groups.items[0].members[0].window_id,
        sizeof(workspace->groups.items[0].members[0].window_id), "%s", "duplicate");
    workspace->groups.items[0].members[1] = workspace->groups.items[0].members[0];
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, NULL) == UMI_STATUS_INVALID_STATE);
    workspace->groups.count = 0U;
    layout->windows[1] = layout->windows[0];
    layout->window_count = 2U;
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, NULL) == UMI_STATUS_INVALID_STATE);
    layout->window_count = 1U;
    (void)memset(layout->windows[0].window_id, 'x', sizeof(layout->windows[0].window_id));
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, NULL) == UMI_STATUS_INVALID_STATE);
    free(before);
    free(workspace);
}

static void testRevisionOverflow(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceCustomisation *before;
    addWindow(workspace, "editor", true, false);
    workspace->revision = UINT64_MAX;
    before = copyWorkspace(workspace);
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_ui_workspace_customisation_place_canvas_window(workspace,
        "editor", 0.0, 0.0, 0.25, 0.5) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    workspace->edit_active = false;
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "new", "New") == UMI_STATUS_CAPACITY_EXCEEDED);
    free(before);
    free(workspace);
}

static void testFullCanvasCapacity(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceCanvasClearResult result;
    size_t index;
    for (index = 0U; index < UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS; ++index) {
        char id[32U];
        (void)snprintf(id, sizeof(id), "panel-%zu", index);
        addWindow(workspace, id, true, index % 2U == 0U);
    }
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, &result) == UMI_STATUS_OK);
    CHECK(result.removed == 32U && result.retained == 32U);
    CHECK(umi_ui_workspace_customisation_commit_edit(workspace) == UMI_STATUS_OK);
    free(workspace);
}

#ifdef UMICOM_TEST_WRAP_MALLOC
static void testAllocationFailure(void)
{
    UmiUiWorkspaceCustomisation *workspace = makeWorkspace();
    UmiUiWorkspaceCustomisation *before;
    addWindow(workspace, "editor", true, false);
    before = copyWorkspace(workspace);
    failAllocation = true;
    CHECK(umi_ui_workspace_customisation_clear_canvas(workspace, NULL) == UMI_STATUS_OUT_OF_MEMORY);
    failAllocation = false;
    CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    CHECK(umi_ui_workspace_customisation_cancel_edit(workspace) == UMI_STATUS_OK);
    (void)memcpy(before, workspace, sizeof(*before));
    failAllocation = true;
    CHECK(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "new", "New") == UMI_STATUS_OUT_OF_MEMORY);
    failAllocation = false;
    CHECK(memcmp(before, workspace, sizeof(*workspace)) == 0);
    free(before);
    free(workspace);
}
#endif

int main(void)
{
    testBlankCreation();
    testBlankFailureAtomicity();
    testBlankCapacity();
    testClearCancelAndCommit();
    testProtectedAndDetached();
    testSharedContextReference();
    testCanvasPlacement();
    testInvalidPlacement();
    testPlacementPolicy();
    testMalformedRecords();
    testRevisionOverflow();
    testFullCanvasCapacity();
#ifdef UMICOM_TEST_WRAP_MALLOC
    testAllocationFailure();
    (void)puts("PASS: 13 canvas transaction test groups, including allocation failure");
#else
    (void)puts("PASS: 12 canvas transaction test groups");
#endif
    return EXIT_SUCCESS;
}
