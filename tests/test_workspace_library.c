/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_library.c
 * PURPOSE: Exercise copied named-layout observations and atomic actions using
 * real customisation/window/context contracts, without files or native UI.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_library.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)

typedef struct LibraryFixture {
    UmiUiWorkspaceCustomisation model;
    UmiUiWorkspaceCustomisation before;
    UmiUiWorkspaceLayout layout;
    UmiUiLayoutLibraryItem preset;
    UmiUiWorkspaceLibrarySnapshot snapshot;
    UmiUiWorkspaceLibrarySnapshot old_snapshot;
} LibraryFixture;

/* Seed real visible/hidden instances and a context/template catalogue so every
 * action can prove it preserves these existing owners byte-for-byte. */
static UmiStatus seed(LibraryFixture *fixture)
{
    UmiUiWorkspaceWindow window = {0};
    UmiUiWindowDescriptor descriptor = {0};
    UmiStatus status;
    umi_ui_workspace_customisation_init(&fixture->model);
    status = umi_ui_workspace_layout_init(&fixture->layout, "umicom.test.primary", "Primary");
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workspace_layout_set_locked(&fixture->layout, false);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(window.window_id, sizeof(window.window_id), "editor");
    (void)snprintf(window.title, sizeof(window.title), "Editor");
    (void)snprintf(window.tool_id, sizeof(window.tool_id), "umicom.test.editor");
    (void)snprintf(window.placement_id, sizeof(window.placement_id), "canvas");
    (void)snprintf(window.context_group_id, sizeof(window.context_group_id), "test.context");
    window.x = 0.1; window.y = 0.1; window.width = 0.4; window.height = 0.5;
    window.visible = true;
    window.closable = true;
    window.resizable = true;
    status = umi_ui_workspace_layout_add_window(&fixture->layout, &window);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(window.window_id, sizeof(window.window_id), "hidden-inspector");
    (void)snprintf(window.title, sizeof(window.title), "Hidden Inspector");
    window.visible = false;
    status = umi_ui_workspace_layout_add_window(&fixture->layout, &window);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workspace_layout_set_locked(&fixture->layout, true);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workspace_customisation_add_layout(&fixture->model, &fixture->layout);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&fixture->preset, 0, sizeof(fixture->preset));
    (void)snprintf(fixture->preset.preset_id, sizeof(fixture->preset.preset_id), "standard");
    (void)snprintf(fixture->preset.category, sizeof(fixture->preset.category), "work");
    (void)snprintf(fixture->preset.description, sizeof(fixture->preset.description), "Original template");
    fixture->preset.layout = fixture->layout;
    status = umi_ui_layout_library_add(&fixture->model.library, &fixture->preset);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workspace_layout_init(&fixture->layout, "umicom.test.secondary", "Secondary");
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workspace_customisation_add_layout(&fixture->model, &fixture->layout);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(descriptor.tool_id, sizeof(descriptor.tool_id), "umicom.test.editor");
    (void)snprintf(descriptor.title, sizeof(descriptor.title), "Editor");
    descriptor.category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
    descriptor.default_width = 0.4;
    descriptor.default_height = 0.5;
    status = umi_ui_window_catalogue_register(&fixture->model.windows, &descriptor);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_window_catalogue_record_open(&fixture->model.windows, descriptor.tool_id, 5U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_window_group_define(&fixture->model.groups, "test.context", "blue", UMI_UI_WINDOW_CONTEXT_FILE);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_window_group_join(&fixture->model.groups, "test.context", "editor", UMI_UI_WINDOW_GROUP_SOURCE);
}

/* Verify that metadata operations never replace catalogue, context or theme. */
static bool shared_owners_unchanged(const LibraryFixture *fixture)
{
    return memcmp(&fixture->model.windows, &fixture->before.windows, sizeof(fixture->model.windows)) == 0 &&
        memcmp(&fixture->model.groups, &fixture->before.groups, sizeof(fixture->model.groups)) == 0 &&
        memcmp(&fixture->model.library, &fixture->before.library, sizeof(fixture->model.library)) == 0 &&
        memcmp(&fixture->model.theme, &fixture->before.theme, sizeof(fixture->model.theme)) == 0;
}

/* Retain both model and output on every rejected request. */
static bool rejected_unchanged(LibraryFixture *fixture,
                                const UmiUiWorkspaceLibraryPolicy *policy,
                                const UmiUiWorkspaceLibraryRequest *request,
                                UmiStatus expected)
{
    UmiStatus status;
    fixture->before = fixture->model;
    fixture->old_snapshot = fixture->snapshot;
    status = umi_ui_workspace_library_apply(&fixture->model, policy, request, &fixture->snapshot);
    return status == expected &&
        memcmp(&fixture->before, &fixture->model, sizeof(fixture->model)) == 0 &&
        memcmp(&fixture->old_snapshot, &fixture->snapshot, sizeof(fixture->snapshot)) == 0;
}

/* Cover library journeys, rollback boundaries and malformed bounded records. */
int main(void)
{
    LibraryFixture *fixture = calloc(1U, sizeof(*fixture));
    UmiUiWorkspaceLibraryPolicy policy = {"umicom.test."};
    UmiUiWorkspaceLibraryPolicy other = {"umicom.other."};
    UmiUiWorkspaceLibraryRequest request = {0};
    char long_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 1U];
    char long_name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY + 1U];
    char identifier[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    const char *invalid_names[] = {
        "\xC0\xAF", "\xED\xA0\x80", "\xF4\x90\x80\x80", "\x80", "\xE2\x82"
    };
    uint64_t revision;
    size_t index;
    int result = 0;
    REQUIRE(fixture != NULL);
    umi_ui_workspace_customisation_init(&fixture->model);
    REQUIRE(umi_ui_workspace_library_snapshot(&fixture->model, &policy, &fixture->snapshot) == UMI_STATUS_OK);
    REQUIRE(fixture->snapshot.layout_count == 0U && fixture->snapshot.customisation_revision == 1U);
    REQUIRE(seed(fixture) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_library_snapshot(&fixture->model, &policy, &fixture->snapshot) == UMI_STATUS_OK);
    REQUIRE(fixture->snapshot.layout_count == 2U);
    REQUIRE(fixture->snapshot.rows[0].window_count == 2U && fixture->snapshot.rows[1].window_count == 0U);
    REQUIRE(fixture->snapshot.rows[0].active && fixture->snapshot.rows[0].locked);
    fixture->old_snapshot = fixture->snapshot;
    fixture->before = fixture->model;
    revision = fixture->model.revision;
    request.action = UMI_UI_WORKSPACE_LIBRARY_DUPLICATE;
    request.target_layout_id = "umicom.test.primary";
    request.new_layout_id = "umicom.test.copy";
    request.name = "Copy \xCE\xBB \xF0\x9F\x9A\x80";
    request.expected_customisation_revision = revision;
    REQUIRE(umi_ui_workspace_library_apply(&fixture->model, &policy, &request, &fixture->snapshot) == UMI_STATUS_OK);
    REQUIRE(fixture->model.revision == revision + 1U && fixture->model.layout_count == 3U);
    REQUIRE(strcmp(fixture->model.active_layout_id, "umicom.test.copy") == 0);
    REQUIRE(fixture->model.layouts[2].locked && fixture->model.layouts[2].revision == 1U);
    REQUIRE(strcmp(fixture->model.layouts[2].name, request.name) == 0);
    REQUIRE(memcmp(fixture->model.layouts[0].windows, fixture->model.layouts[2].windows,
                   sizeof(fixture->model.layouts[0].windows)) == 0);
    REQUIRE(fixture->model.layouts[0].revision == fixture->before.layouts[0].revision);
    REQUIRE(shared_owners_unchanged(fixture));
    REQUIRE(fixture->old_snapshot.layout_count == 2U && strcmp(fixture->old_snapshot.rows[0].name, "Primary") == 0);

    request.expected_customisation_revision = fixture->model.revision;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_ALREADY_EXISTS));
    request.action = UMI_UI_WORKSPACE_LIBRARY_RENAME;
    request.target_layout_id = "umicom.test.primary";
    request.name = "Renamed primary";
    request.expected_customisation_revision = fixture->model.revision;
    fixture->before = fixture->model;
    REQUIRE(umi_ui_workspace_library_apply(&fixture->model, &policy, &request, &fixture->snapshot) == UMI_STATUS_OK);
    REQUIRE(strcmp(fixture->model.active_layout_id, "umicom.test.copy") == 0);
    REQUIRE(fixture->model.layouts[0].locked && fixture->model.layouts[0].revision == fixture->before.layouts[0].revision + 1U);
    REQUIRE(shared_owners_unchanged(fixture));
    request.expected_customisation_revision = fixture->model.revision;
    fixture->before = fixture->model;
    REQUIRE(umi_ui_workspace_library_apply(&fixture->model, &policy, &request, NULL) == UMI_STATUS_OK);
    REQUIRE(memcmp(&fixture->before, &fixture->model, sizeof(fixture->model)) == 0);
    request.action = UMI_UI_WORKSPACE_LIBRARY_ACTIVATE;
    request.expected_customisation_revision = fixture->model.revision;
    REQUIRE(umi_ui_workspace_library_apply(&fixture->model, &policy, &request, NULL) == UMI_STATUS_OK);
    REQUIRE(strcmp(fixture->model.active_layout_id, "umicom.test.primary") == 0);
    request.expected_customisation_revision = fixture->model.revision;
    fixture->before = fixture->model;
    REQUIRE(umi_ui_workspace_library_apply(&fixture->model, &policy, &request, NULL) == UMI_STATUS_OK);
    REQUIRE(memcmp(&fixture->before, &fixture->model, sizeof(fixture->model)) == 0);

    request.action = UMI_UI_WORKSPACE_LIBRARY_REMOVE;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_PERMISSION_DENIED));
    request.confirmed = true;
    fixture->before = fixture->model;
    REQUIRE(umi_ui_workspace_library_apply(&fixture->model, &policy, &request, &fixture->snapshot) == UMI_STATUS_OK);
    REQUIRE(fixture->model.layout_count == 2U && strcmp(fixture->model.active_layout_id, "umicom.test.secondary") == 0);
    REQUIRE(shared_owners_unchanged(fixture));
    request.target_layout_id = "umicom.test.copy";
    request.expected_customisation_revision = fixture->model.revision;
    REQUIRE(umi_ui_workspace_library_apply(&fixture->model, &policy, &request, NULL) == UMI_STATUS_OK);
    REQUIRE(strcmp(fixture->model.active_layout_id, "umicom.test.secondary") == 0);
    request.target_layout_id = "umicom.test.secondary";
    request.expected_customisation_revision = fixture->model.revision;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_STATE));

    /* Editing blocks all four mutations, but the current library is readable. */
    REQUIRE(umi_ui_workspace_customisation_begin_edit(&fixture->model) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_library_snapshot(&fixture->model, &policy, &fixture->snapshot) == UMI_STATUS_OK);
    REQUIRE(fixture->snapshot.editing);
    for (index = UMI_UI_WORKSPACE_LIBRARY_DUPLICATE; index <= UMI_UI_WORKSPACE_LIBRARY_ACTIVATE; ++index) {
        request.action = (UmiUiWorkspaceLibraryAction)index;
        request.expected_customisation_revision = fixture->model.revision;
        REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_BUSY));
    }
    REQUIRE(umi_ui_workspace_customisation_cancel_edit(&fixture->model) == UMI_STATUS_OK);
    request.action = UMI_UI_WORKSPACE_LIBRARY_RENAME;
    request.name = "Changed";
    request.expected_customisation_revision = fixture->model.revision - 1U;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_STATE));
    request.expected_customisation_revision = fixture->model.revision;
    REQUIRE(rejected_unchanged(fixture, &other, &request, UMI_STATUS_PERMISSION_DENIED));
    request.target_layout_id = "umicom.other.layout";
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_PERMISSION_DENIED));
    request.target_layout_id = "umicom.test.unknown";
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_NOT_FOUND));
    request.target_layout_id = "umicom.test.secondary";
    request.name = "   ";
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_ARGUMENT));
    for (index = 0U; index < sizeof(invalid_names) / sizeof(invalid_names[0]); ++index) {
        request.name = invalid_names[index];
        REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_ARGUMENT));
    }
    memset(long_name, 'n', sizeof(long_name) - 1U);
    long_name[sizeof(long_name) - 1U] = '\0';
    request.name = long_name;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_CAPACITY_EXCEEDED));
    memset(long_id, 'i', sizeof(long_id) - 1U);
    long_id[sizeof(long_id) - 1U] = '\0';
    request.target_layout_id = long_id;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_CAPACITY_EXCEEDED));

    /* Corrupt fixed records fail before string lookup or geometry diagnostics. */
    REQUIRE(seed(fixture) == UMI_STATUS_OK);
    request.action = UMI_UI_WORKSPACE_LIBRARY_RENAME;
    request.target_layout_id = "umicom.test.primary";
    request.name = "Safe";
    request.expected_customisation_revision = fixture->model.revision;
    fixture->model.layouts[0].window_count = UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS + 1U;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_STATE));
    fixture->model.layouts[0].window_count = 2U;
    memset(fixture->model.layouts[0].windows[1].title, 'x', sizeof(fixture->model.layouts[0].windows[1].title));
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_STATE));
    fixture->model.layouts[0].windows[1].title[0] = '\0';
    fixture->model.layouts[0].windows[1].width = NAN;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_STATE));
    fixture->model.layouts[0].windows[1].width = 0.4;
    fixture->model.groups.items[0].member_count = UMI_UI_WINDOW_GROUP_MAX_MEMBERS + 1U;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_STATE));
    fixture->model.groups.items[0].member_count = 1U;
    fixture->model.layout_count = UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS + 1U;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_STATE));
    fixture->model.layout_count = 2U;
    memset(fixture->model.layouts[1].layout_id, 'x', sizeof(fixture->model.layouts[1].layout_id));
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_STATE));
    REQUIRE(seed(fixture) == UMI_STATUS_OK);
    (void)snprintf(fixture->model.layouts[1].layout_id, sizeof(fixture->model.layouts[1].layout_id), "umicom.test.primary");
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_STATE));
    REQUIRE(seed(fixture) == UMI_STATUS_OK);
    (void)snprintf(fixture->model.active_layout_id, sizeof(fixture->model.active_layout_id), "umicom.test.missing");
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_INVALID_STATE));
    REQUIRE(seed(fixture) == UMI_STATUS_OK);
    (void)memcpy(fixture->model.layouts[0].name, "\xC0\xAF", sizeof("\xC0\xAF"));
    fixture->old_snapshot = fixture->snapshot;
    REQUIRE(umi_ui_workspace_library_snapshot(&fixture->model, &policy, &fixture->snapshot) == UMI_STATUS_INVALID_STATE);
    REQUIRE(memcmp(&fixture->old_snapshot, &fixture->snapshot, sizeof(fixture->snapshot)) == 0);
    REQUIRE(seed(fixture) == UMI_STATUS_OK);
    fixture->before = fixture->model;
    REQUIRE(umi_ui_workspace_library_snapshot(&fixture->model, &policy,
        (UmiUiWorkspaceLibrarySnapshot *)(void *)&fixture->model) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_ui_workspace_library_apply(&fixture->model, &policy, &request,
        (UmiUiWorkspaceLibrarySnapshot *)(void *)&fixture->model) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(memcmp(&fixture->before, &fixture->model, sizeof(fixture->model)) == 0);

    /* Overflow protection applies only to revisions the operation changes. */
    fixture->model.revision = UINT64_MAX;
    request.expected_customisation_revision = UINT64_MAX;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_CAPACITY_EXCEEDED));
    request.name = fixture->model.layouts[0].name;
    REQUIRE(umi_ui_workspace_library_apply(&fixture->model, &policy, &request, NULL) == UMI_STATUS_OK);
    REQUIRE(fixture->model.revision == UINT64_MAX);
    fixture->model.revision = 12U;
    fixture->model.layouts[0].revision = UINT64_MAX;
    request.expected_customisation_revision = 12U;
    request.name = "Another name";
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_CAPACITY_EXCEEDED));
    request.action = UMI_UI_WORKSPACE_LIBRARY_DUPLICATE;
    request.new_layout_id = "umicom.test.alias-copy";
    request.name = fixture->model.layouts[1].name;
    request.target_layout_id = fixture->model.layouts[0].layout_id;
    fixture->before = fixture->model;
    REQUIRE(umi_ui_workspace_library_apply(&fixture->model, &policy, &request, NULL) == UMI_STATUS_OK);
    REQUIRE(fixture->model.revision == 13U && fixture->model.layouts[2].revision == 1U);
    REQUIRE(memcmp(&fixture->model.layouts[0], &fixture->before.layouts[0],
                   sizeof(fixture->model.layouts[0])) == 0);
    REQUIRE(strcmp(fixture->model.layouts[2].name, "Secondary") == 0);

    /* Capacity comes from the sole model constant, never a parallel limit. */
    REQUIRE(seed(fixture) == UMI_STATUS_OK);
    for (index = fixture->model.layout_count; index < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS; ++index) {
        (void)snprintf(identifier, sizeof(identifier), "umicom.test.layout%zu", index);
        REQUIRE(umi_ui_workspace_layout_init(&fixture->layout, identifier, "Additional") == UMI_STATUS_OK);
        REQUIRE(umi_ui_workspace_customisation_add_layout(&fixture->model, &fixture->layout) == UMI_STATUS_OK);
    }
    REQUIRE(umi_ui_workspace_library_snapshot(&fixture->model, &policy, &fixture->snapshot) == UMI_STATUS_OK);
    REQUIRE(fixture->snapshot.layout_count == UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS);
    request.action = UMI_UI_WORKSPACE_LIBRARY_DUPLICATE;
    request.target_layout_id = "umicom.test.primary";
    request.new_layout_id = "umicom.test.overflow";
    request.name = "Full";
    request.expected_customisation_revision = fixture->model.revision;
    REQUIRE(rejected_unchanged(fixture, &policy, &request, UMI_STATUS_CAPACITY_EXCEEDED));
cleanup:
    free(fixture);
    return result;
}
