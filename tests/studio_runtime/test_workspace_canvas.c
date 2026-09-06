/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_workspace_canvas.c
 *
 * PURPOSE:
 *   Check native Studio panel identities, shared edit ownership and one-way
 *   shell projection without opening any windows or starting external tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/suite_layout/render_plan.h"
#include "umicom/studio_runtime/layout_catalogue.h"
#include "umicom/studio_runtime/workspace_canvas.h"
#include "umicom/test_runtime/check.h"

/* Run deterministic model checks; none of these scenarios require a display. */
int main(void)
{
    /* Workspace records contain every saved layout. Keep these fixtures on the
     * heap so the test is safe with ordinary Windows thread stack limits. */
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *before = calloc(1U, sizeof(*before));
    UmiApplicationSuiteLayoutRenderPlan *plan = calloc(1U, sizeof(*plan));
    UmiApplicationShellLayout *shell_layout = calloc(1U, sizeof(*shell_layout));
    UmiApplicationShellLayout *shell_before = calloc(1U, sizeof(*shell_before));
    UmiApplicationShellRegistry *registry = NULL;
    UmiApplicationShellState shell_state, state_before;
    UmiUiWorkspaceLayout *layout;
    const UmiUiWorkspaceWindow *window;
    UmiUiWorkspacePanelSettings settings;
    UmiApplicationSuiteLayoutRect rect = {0.12, 0.18, 0.45, 0.42};
    UmiStudioRuntimeSurfaceKind kind = UMI_STUDIO_SURFACE_EDITOR;
    uint64_t revision;
    size_t index, preset_index;
    UMI_TEST_REQUIRE(model != NULL && before != NULL && plan != NULL &&
        shell_layout != NULL && shell_before != NULL);

    umi_ui_workspace_customisation_init(model);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_seed(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(model->windows.count == umi_studio_surface_catalogue_count());
    UMI_TEST_REQUIRE(model->layout_count == umi_studio_layout_catalogue_count());
    UMI_TEST_REQUIRE(model->layout_count < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS);
    UMI_TEST_REQUIRE(model->groups.count == 5U);
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_seed(model) == UMI_STATUS_ALREADY_EXISTS);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);

    /* Every real tool has a unique, reversible identity and every native preset
     * contains the exact visibility declared by the existing preset catalogue. */
    for (index = 0U; index < umi_studio_surface_catalogue_count(); ++index) {
        const UmiStudioRuntimeSurfaceBinding *binding = umi_studio_surface_catalogue_at(index);
        const char *id = umi_studio_runtime_workspace_surface_id(binding->kind);
        UMI_TEST_REQUIRE(id != NULL);
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_surface_kind(id, &kind) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(kind == binding->kind);
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_surface_kind(binding->ide_surface_id, &kind) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(kind == binding->kind);
        UMI_TEST_REQUIRE(umi_ui_window_catalogue_find(&model->windows, id) != NULL);
        for (preset_index = 0U; preset_index < model->layout_count; ++preset_index) {
            const UmiStudioRuntimeLayoutPresetDefinition *preset = umi_studio_layout_catalogue_at(preset_index);
            const UmiUiWorkspaceLayout *saved = &model->layouts[preset_index];
            window = umi_ui_workspace_layout_find_window(saved, id);
            UMI_TEST_REQUIRE(window != NULL && strcmp(window->tool_id, id) == 0);
            UMI_TEST_REQUIRE(saved->window_count == umi_studio_surface_catalogue_count());
            UMI_TEST_REQUIRE(window->visible == (umi_studio_layout_preset_contains_surface(preset, binding->kind) != 0));
        }
    }
    kind = UMI_STUDIO_SURFACE_EDITOR;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_surface_kind("unknown", &kind) == UMI_STATUS_NOT_FOUND);
    UMI_TEST_REQUIRE(kind == UMI_STUDIO_SURFACE_EDITOR);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_surface_id((UmiStudioRuntimeSurfaceKind)0) == NULL);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_select(model, "default") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_select(model, "org.umicom.trader.default") == UMI_STATUS_INVALID_ARGUMENT);
    layout = umi_ui_workspace_customisation_active(model);
    UMI_TEST_REQUIRE(strcmp(umi_ui_workspace_layout_find_window(layout, "editor")->context_group_id,
        "studio.context.development") == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_layout_find_window(layout, "output")->context_group_id[0] == '\0');
    {
        UmiUiWorkspaceWindow *problems = umi_ui_workspace_layout_find_window_mutable(layout, "problems");
        /* A restored record cannot evade a non-floating product policy merely
         * by waiting until the user reopens its hidden surface. */
        problems->visible = false;
        problems->floating = true;
        *before = *model;
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_PROBLEMS, true) == UMI_STATUS_PERMISSION_DENIED);
        UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
        problems->floating = false;
        problems->visible = true;
    }

    /* A user canvas begins genuinely empty, then opens the same real Editor
     * singleton as a free-positioned panel. Its document buffers are not owned
     * or recreated by any of these model operations. */
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_blank(model,
        "umicom.studio.layout.custom", "My workspace") == UMI_STATUS_OK);
    layout = umi_ui_workspace_customisation_active(model);
    UMI_TEST_REQUIRE(layout != NULL && model->edit_active && !layout->locked && layout->window_count == 0U);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_select(model, "default") == UMI_STATUS_BUSY);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, true) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(layout->window_count == 1U);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, true) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(layout->window_count == 1U);
    window = umi_ui_workspace_layout_find_window(layout, "editor");
    UMI_TEST_REQUIRE(window != NULL && strcmp(window->placement_id, "canvas") == 0);
    revision = layout->revision;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_place_canvas(model, "editor", &rect, revision) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(window->x == rect.x && window->width == rect.width);
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_place_canvas(model, "editor", &rect, revision) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    rect.x = NAN;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_place_canvas(model, "editor", &rect, layout->revision) == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    rect.x = 0.12;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_pinned(model, "editor", true) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_place_canvas(model, "editor", &rect, layout->revision) == UMI_STATUS_PERMISSION_DENIED);
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, false) == UMI_STATUS_PERMISSION_DENIED);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_pinned(model, "editor", false) == UMI_STATUS_OK);
    {
        UmiUiWorkspaceWindow *editor = umi_ui_workspace_layout_find_window_mutable(layout, "editor");
        /* A catalogue Hide request must not bypass protected instance flags.
         * Rejection leaves geometry, visibility and the edit baseline intact. */
        editor->closable = false;
        *before = *model;
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, false) == UMI_STATUS_PERMISSION_DENIED);
        UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
        editor->closable = true;
        layout->locked = true;
        *before = *model;
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, false) == UMI_STATUS_PERMISSION_DENIED);
        UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
        layout->locked = false;
    }
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_maximised(model, "editor", true) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(window->maximised && window->x == rect.x);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_maximised(model, "editor", false) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, false) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, true) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(window->x == rect.x && layout->window_count == 1U);
    UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(plan->canvas_item_count == 1U && plan->visible_window_count == 1U);

    /* Native views with an existing restrictive product policy cannot bypass
     * it merely by using a different adapter. Problems supports docking, but
     * does not advertise detached floating windows in its product metadata. */
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_PROBLEMS, true) == UMI_STATUS_OK);
    settings = umi_ui_workspace_panel_settings_default("problems");
    settings.floating = true;
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_apply_panel_settings(model, &settings) == UMI_STATUS_PERMISSION_DENIED);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_cancel_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(layout->window_count == 0U && layout->locked && !model->edit_active);

    /* Menu reopening outside layout editing uses a short atomic transaction.
     * The pre-existing blank layout remains a saved, locked layout afterwards. */
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, true) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(!model->edit_active && layout->locked && layout->window_count == 1U);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_pinned(model, "editor", true) == UMI_STATUS_INVALID_STATE);
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, false) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    /* An unlocked record without an edit session is not removal authority. */
    layout->locked = false;
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, false) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    layout->locked = true;
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_blank(model, "umicom.studio.layout.", "Invalid") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_blank(model, "umicom.studio.layout.custom", "Duplicate") == UMI_STATUS_ALREADY_EXISTS);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    revision = model->revision;
    model->revision = UINT64_MAX;
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, false) == UMI_STATUS_CAPACITY_EXCEEDED);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    model->revision = revision;
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_cancel_edit(model) == UMI_STATUS_OK);

    /* Command services get a one-way semantic projection. Free geometry stays
     * in the native model and missing bindings fail before outputs are changed. */
    UMI_TEST_REQUIRE(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    umi_application_shell_state_init(&shell_state);
    umi_application_shell_layout_init(shell_layout, "test", "Test");
    UMI_TEST_REQUIRE(umi_studio_surface_catalogue_install(registry, shell_layout) == UMI_STATUS_OK);
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_project_shell(model, registry, &shell_state, shell_layout) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    UMI_TEST_REQUIRE(shell_state.active_view_id[0] != '\0');
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_EDITOR, false) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_project_shell(model, registry, &shell_state, shell_layout) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(shell_state.active_view_id[0] == '\0' && shell_state.focused_view_id[0] == '\0');
    UMI_TEST_REQUIRE(!shell_state.primary_sidebar_visible && !shell_state.secondary_sidebar_visible && !shell_state.bottom_panel_visible);
    UMI_TEST_REQUIRE(umi_application_shell_registry_remove(registry,
        umi_studio_surface_catalogue_for_kind(UMI_STUDIO_SURFACE_EDITOR)->fallback_contribution_id) == UMI_STATUS_OK);
    state_before = shell_state;
    *shell_before = *shell_layout;
    revision = umi_application_shell_registry_revision(registry);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_project_shell(model, registry, &shell_state, shell_layout) == UMI_STATUS_NOT_FOUND);
    UMI_TEST_REQUIRE(memcmp(&shell_state, &state_before, sizeof(shell_state)) == 0);
    UMI_TEST_REQUIRE(memcmp(shell_layout, shell_before, sizeof(*shell_layout)) == 0);
    UMI_TEST_REQUIRE(umi_application_shell_registry_revision(registry) == revision);

    umi_application_shell_registry_destroy(registry);
    free(shell_before); free(shell_layout); free(plan); free(before); free(model);
    return EXIT_SUCCESS;
}
