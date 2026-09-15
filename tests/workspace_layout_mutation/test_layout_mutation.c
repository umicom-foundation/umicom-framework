/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workspace_layout_mutation/test_layout_mutation.c
 *
 * PURPOSE:
 *   Verify failed and aliased workspace edits against the production C model,
 *   preserving layouts, tab associations and existing panel operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_layout.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Checks stay active under NDEBUG. The outer fixture always owns cleanup. */
#define REQUIRE(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        return 1; \
    } \
} while (0)

typedef struct Fixture {
    UmiUiWorkspaceLayout layout;
    UmiUiWorkspaceLayout before;
    UmiUiWorkspaceLayout source;
} Fixture;

/* Fixtures are heap-owned, not large automatic objects on native thread stacks.
 * Initialise independently of the operations under test and include tail bytes
 * in every failure comparison, so a partial write cannot hide behind a status. */
static void Reset(Fixture *fixture)
{
    UmiUiWorkspaceLayout *layout = &fixture->layout;
    UmiUiWorkspaceWindow *window;
    (void)memset(fixture, 0, sizeof(*fixture));
    (void)memcpy(layout->layout_id, "workspace.saved", sizeof("workspace.saved"));
    (void)memcpy(layout->name, "Saved workspace", sizeof("Saved workspace"));
    layout->revision = 42U;
    layout->window_count = 1U;
    window = &layout->windows[0];
    (void)memcpy(window->window_id, "editor", sizeof("editor"));
    (void)memcpy(window->tool_id, "editor.tool", sizeof("editor.tool"));
    (void)memcpy(window->title, "Unsaved editor", sizeof("Unsaved editor"));
    (void)memcpy(window->group_id, "prefix.stack", sizeof("prefix.stack"));
    (void)memcpy(window->stack_id, "prefix.stack", sizeof("prefix.stack"));
    (void)memcpy(window->placement_id, "canvas", sizeof("canvas"));
    (void)memcpy(window->context_group_id, "prefix.blue", sizeof("prefix.blue"));
    window->x = 0.1; window->y = 0.1;
    window->width = 0.5; window->height = 0.6;
    window->visible = true; window->closable = true; window->resizable = true;
    window->z_order = 3;
    (void)memcpy(&fixture->before, layout, sizeof(*layout));
    (void)memcpy(&fixture->source, layout, sizeof(*layout));
    (void)memcpy(fixture->source.name, "Other workspace", sizeof("Other workspace"));
    fixture->source.windows[0].x = 0.2;
}

static int Unchanged(const Fixture *fixture)
{
    return memcmp(&fixture->layout, &fixture->before,
                  sizeof(fixture->layout)) == 0;
}

static void SaveBefore(Fixture *fixture)
{
    (void)memcpy(&fixture->before, &fixture->layout, sizeof(fixture->before));
}

/* Produce a terminated boundary value, including the exact full-field length. */
static void Fill(char *text, size_t count)
{
    (void)memset(text, 'X', count);
    text[count] = '\0';
}

static int CheckInitialisation(Fixture *fixture)
{
    char overId[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 1U];
    char overName[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY + 1U];
    Fill(overId, sizeof(overId) - 1U);
    Fill(overName, sizeof(overName) - 1U);
    REQUIRE(umi_ui_workspace_layout_init(&fixture->layout, overId, "New") == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Unchanged(fixture));
    REQUIRE(umi_ui_workspace_layout_init(&fixture->layout, "new.id", overName) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Unchanged(fixture));
    REQUIRE(umi_ui_workspace_layout_init(&fixture->layout, "", "New") == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(Unchanged(fixture));
    REQUIRE(umi_ui_workspace_layout_init(&fixture->layout, "id", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(Unchanged(fixture));
    /* Existing fields are valid arguments; successful init still clears panels. */
    REQUIRE(umi_ui_workspace_layout_init(&fixture->layout,
        fixture->layout.layout_id, fixture->layout.name) == UMI_STATUS_OK);
    REQUIRE(strcmp(fixture->layout.layout_id, "workspace.saved") == 0);
    REQUIRE(strcmp(fixture->layout.name, "Saved workspace") == 0);
    REQUIRE(fixture->layout.window_count == 0U && fixture->layout.locked);
    REQUIRE(fixture->layout.revision == 1U);
    return 0;
}

static int CheckClone(Fixture *fixture)
{
    char overId[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 1U];
    char overName[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY + 1U];
    Fill(overId, sizeof(overId) - 1U); Fill(overName, sizeof(overName) - 1U);
    REQUIRE(umi_ui_workspace_layout_clone(&fixture->source, overId, "Copy",
        &fixture->layout) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Unchanged(fixture));
    REQUIRE(umi_ui_workspace_layout_clone(&fixture->source, "copy", overName,
        &fixture->layout) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Unchanged(fixture));
    REQUIRE(umi_ui_workspace_layout_clone(&fixture->layout, overId, "Copy",
        &fixture->layout) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Unchanged(fixture));
    REQUIRE(umi_ui_workspace_layout_clone(&fixture->source,
        fixture->layout.layout_id, fixture->layout.name,
        &fixture->layout) == UMI_STATUS_OK);
    REQUIRE(strcmp(fixture->layout.layout_id, "workspace.saved") == 0);
    REQUIRE(strcmp(fixture->layout.name, "Saved workspace") == 0);
    REQUIRE(fixture->layout.windows[0].x == 0.2);
    REQUIRE(fixture->layout.revision == 1U);
    REQUIRE(strcmp(fixture->source.name, "Other workspace") == 0);
    /* Exact self-clone and suffix aliases must not invoke overlapping snprintf. */
    REQUIRE(umi_ui_workspace_layout_clone(&fixture->layout,
        fixture->layout.layout_id + 10U, fixture->layout.name + 6U,
        &fixture->layout) == UMI_STATUS_OK);
    REQUIRE(strcmp(fixture->layout.layout_id, "saved") == 0);
    REQUIRE(strcmp(fixture->layout.name, "workspace") == 0);
    REQUIRE(fixture->layout.window_count == 1U);
    SaveBefore(fixture);
    fixture->source.window_count = UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS + 1U;
    REQUIRE(umi_ui_workspace_layout_clone(&fixture->source, "id", "Name",
        &fixture->layout) == UMI_STATUS_INVALID_STATE);
    REQUIRE(Unchanged(fixture));
    return 0;
}

static int CheckRename(Fixture *fixture)
{
    char text[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY + 1U];
    Fill(text, sizeof(text) - 1U);
    REQUIRE(umi_ui_workspace_layout_rename(&fixture->layout, text) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Unchanged(fixture));
    REQUIRE(umi_ui_workspace_layout_rename(&fixture->layout,
        fixture->layout.name) == UMI_STATUS_OK);
    REQUIRE(strcmp(fixture->layout.name, "Saved workspace") == 0);
    REQUIRE(fixture->layout.revision == 43U);
    REQUIRE(umi_ui_workspace_layout_rename(&fixture->layout,
        fixture->layout.name + 6U) == UMI_STATUS_OK);
    REQUIRE(strcmp(fixture->layout.name, "workspace") == 0);
    text[sizeof(text) - 2U] = '\0';
    REQUIRE(umi_ui_workspace_layout_rename(&fixture->layout, text) == UMI_STATUS_OK);
    REQUIRE(strlen(fixture->layout.name) == sizeof(fixture->layout.name) - 1U);
    SaveBefore(fixture);
    REQUIRE(umi_ui_workspace_layout_rename(&fixture->layout, "") == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(Unchanged(fixture));
    return 0;
}

typedef UmiStatus (*TextOperation)(UmiUiWorkspaceLayout *, const char *, const char *);

/* Exercise the same rejection/alias matrix for every panel string mutation.
 * For group/stack edits, verify both legacy and canonical fields together. */
static int CheckText(Fixture *fixture, TextOperation operation, char *field, bool paired,
                     bool allowEmpty)
{
    char text[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 1U];
    Fill(text, sizeof(text) - 1U);
    REQUIRE(operation(&fixture->layout, "editor", text) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Unchanged(fixture));
    /* A readable array with no NUL within the limit is rejected without strlen. */
    text[sizeof(text) - 1U] = 'X';
    REQUIRE(operation(&fixture->layout, "editor", text) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Unchanged(fixture));
    REQUIRE(operation(&fixture->layout, "editor", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(Unchanged(fixture));
    REQUIRE(operation(&fixture->layout, "missing", "value") == UMI_STATUS_NOT_FOUND);
    REQUIRE(Unchanged(fixture));
    REQUIRE(operation(&fixture->layout, "editor", field) == UMI_STATUS_OK);
    REQUIRE(fixture->layout.revision == 43U);
    REQUIRE(field[0] != '\0');
    REQUIRE(operation(&fixture->layout, "editor", "prefix.value") == UMI_STATUS_OK);
    REQUIRE(operation(&fixture->layout, "editor", field + 7U) == UMI_STATUS_OK);
    REQUIRE(strcmp(field, "value") == 0);
    if (paired) {
        REQUIRE(strcmp(fixture->layout.windows[0].group_id, "value") == 0);
        REQUIRE(strcmp(fixture->layout.windows[0].stack_id, "value") == 0);
        /* The argument can be the OTHER destination's interior suffix. */
        REQUIRE(operation(&fixture->layout, "editor", "prefix.other") == UMI_STATUS_OK);
        REQUIRE(operation(&fixture->layout, "editor",
            fixture->layout.windows[0].group_id + 7U) == UMI_STATUS_OK);
        REQUIRE(strcmp(fixture->layout.windows[0].group_id, "other") == 0);
        REQUIRE(strcmp(fixture->layout.windows[0].stack_id, "other") == 0);
    }
    Fill(text, sizeof(text) - 2U);
    REQUIRE(operation(&fixture->layout, "editor", text) == UMI_STATUS_OK);
    REQUIRE(strlen(field) == UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY - 1U);
    SaveBefore(fixture);
    REQUIRE(operation(&fixture->layout, "editor", "") ==
        (allowEmpty ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
    if (allowEmpty) REQUIRE(field[0] == '\0');
    else REQUIRE(Unchanged(fixture));
    return 0;
}

static int CheckGroup(Fixture *fixture)
{
    return CheckText(fixture, umi_ui_workspace_layout_set_group,
        fixture->layout.windows[0].group_id, true, false);
}
static int CheckStack(Fixture *fixture)
{
    return CheckText(fixture, umi_ui_workspace_layout_set_stack,
        fixture->layout.windows[0].stack_id, true, false);
}
static int CheckPlacement(Fixture *fixture)
{
    return CheckText(fixture, umi_ui_workspace_layout_set_placement,
        fixture->layout.windows[0].placement_id, false, false);
}
static int CheckContext(Fixture *fixture)
{
    return CheckText(fixture, umi_ui_workspace_layout_set_context_group,
        fixture->layout.windows[0].context_group_id, false, true);
}

static int CheckLocked(Fixture *fixture)
{
    const TextOperation operations[] = {
        umi_ui_workspace_layout_set_group, umi_ui_workspace_layout_set_stack,
        umi_ui_workspace_layout_set_placement, umi_ui_workspace_layout_set_context_group
    };
    fixture->layout.locked = true;
    SaveBefore(fixture);
    for (size_t index = 0U; index < sizeof(operations) / sizeof(operations[0]); ++index) {
        REQUIRE(operations[index](&fixture->layout, "editor", "new") == UMI_STATUS_PERMISSION_DENIED);
        REQUIRE(Unchanged(fixture));
    }
    REQUIRE(umi_ui_workspace_layout_rename(&fixture->layout, "New") == UMI_STATUS_PERMISSION_DENIED);
    REQUIRE(Unchanged(fixture));
    REQUIRE(umi_ui_workspace_layout_place_window(&fixture->layout, "editor", 0, 0, 1, 1) == UMI_STATUS_PERMISSION_DENIED);
    REQUIRE(umi_ui_workspace_layout_remove_window(&fixture->layout, "editor") == UMI_STATUS_PERMISSION_DENIED);
    REQUIRE(Unchanged(fixture));
    return 0;
}

static int CheckUtf8(Fixture *fixture)
{
    char text[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY + 1U];
    /* 95 Greek lambda characters plus ASCII fit in 191 bytes; 96 do not. */
    for (size_t index = 0U; index < 96U; ++index) {
        text[index * 2U] = (char)0xce;
        text[index * 2U + 1U] = (char)0xbb;
    }
    text[192] = '\0';
    REQUIRE(umi_ui_workspace_layout_rename(&fixture->layout, text) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Unchanged(fixture));
    text[190] = 'A'; text[191] = '\0';
    REQUIRE(umi_ui_workspace_layout_rename(&fixture->layout, text) == UMI_STATUS_OK);
    REQUIRE(strcmp(fixture->layout.name, text) == 0);
    REQUIRE(strlen(fixture->layout.name) == 191U);
    REQUIRE(strcmp(fixture->layout.windows[0].title, "Unsaved editor") == 0);
    return 0;
}

static int CheckJourney(Fixture *fixture)
{
    UmiUiWorkspaceWindow extra = fixture->layout.windows[0];
    char tooLong[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY + 1U];
    char reason[192];
    uint64_t revision;
    (void)memcpy(extra.window_id, "chart", sizeof("chart"));
    REQUIRE(umi_ui_workspace_layout_add_window(&fixture->layout, &extra) == UMI_STATUS_OK);
    (void)memcpy(extra.window_id, "output", sizeof("output"));
    REQUIRE(umi_ui_workspace_layout_add_window(&fixture->layout, &extra) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_place_window(&fixture->layout, "chart", 0.6, 0.1, 0.4, 0.5) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_floating(&fixture->layout, "chart", true) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_visible(&fixture->layout, "output", false) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_visible(&fixture->layout, "output", true) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_maximised(&fixture->layout, "editor", true) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_maximised(&fixture->layout, "editor", false) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_pinned(&fixture->layout, "editor", true) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_context_group(&fixture->layout, "chart", "shared.blue") == UMI_STATUS_OK);
    SaveBefore(fixture);
    Fill(tooLong, sizeof(tooLong) - 1U);
    REQUIRE(umi_ui_workspace_layout_rename(&fixture->layout, tooLong) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Unchanged(fixture));
    REQUIRE(umi_ui_workspace_layout_place_window(&fixture->layout, "chart", NAN, 0, 1, 1) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_ui_workspace_layout_place_window(&fixture->layout, "chart", 0, 0, INFINITY, 1) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(Unchanged(fixture));
    revision = fixture->layout.revision;
    REQUIRE(umi_ui_workspace_layout_rename(&fixture->layout, "Research") == UMI_STATUS_OK);
    REQUIRE(fixture->layout.revision == revision + 1U);
    REQUIRE(umi_ui_workspace_layout_validate(&fixture->layout, reason, sizeof(reason)) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_remove_window(&fixture->layout, "output") == UMI_STATUS_OK);
    REQUIRE(fixture->layout.window_count == 2U);
    REQUIRE(umi_ui_workspace_layout_find_window(&fixture->layout, "editor") != NULL);
    REQUIRE(umi_ui_workspace_layout_find_window(&fixture->layout, "chart")->floating);
    REQUIRE(umi_ui_workspace_layout_count_tool(&fixture->layout, "editor.tool") == 2U);
    REQUIRE(umi_ui_workspace_layout_set_locked(&fixture->layout, true) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_remove_window(&fixture->layout, "chart") == UMI_STATUS_PERMISSION_DENIED);
    return 0;
}

/* Invalid test selection is an error, never an empty successful CTest run. */
int main(int argc, char **argv)
{
    static const struct {
        const char *name;
        int (*run)(Fixture *);
    } cases[] = {
        {"initialisation", CheckInitialisation}, {"clone", CheckClone},
        {"rename", CheckRename}, {"group", CheckGroup}, {"stack", CheckStack},
        {"placement", CheckPlacement}, {"context", CheckContext},
        {"locked", CheckLocked}, {"utf8", CheckUtf8}, {"journey", CheckJourney}
    };
    Fixture *fixture;
    size_t selected = 0U;
    int failures = 0;
    if (argc != 2) {
        (void)fprintf(stderr, "Usage: %s <case|all>\n", argv[0]);
        return 2;
    }
    fixture = malloc(sizeof(*fixture));
    if (fixture == NULL) return 2;
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        if (strcmp(argv[1], "all") != 0 && strcmp(argv[1], cases[index].name) != 0) continue;
        Reset(fixture);
        ++selected;
        const int result = cases[index].run(fixture);
        (void)printf("%s: %s\n", cases[index].name, result == 0 ? "PASS" : "FAIL");
        if (result != 0) ++failures;
    }
    free(fixture);
    if (selected == 0U) return 2;
    return failures == 0 ? 0 : 1;
}
