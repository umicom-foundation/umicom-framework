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

/* A restored default may legitimately contain older user geometry. Creating
 * today's default must add a separate named record, never reset that record. */
static int test_named_product_default(void)
{
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *before = calloc(1U, sizeof(*before));
    UmiUiWorkspaceCustomisation *fresh = calloc(1U, sizeof(*fresh));
    const char *new_id = "umicom.studio.layout.product-default-1";
    const UmiUiWorkspaceLayout *layout;
    const UmiUiWorkspaceLayout *canonical;
    char too_long[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 1U];
    size_t index;
    UMI_TEST_REQUIRE(model != NULL && before != NULL && fresh != NULL);
    umi_ui_workspace_customisation_init(model);
    umi_ui_workspace_customisation_init(fresh);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_seed(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_seed(fresh) == UMI_STATUS_OK);
    canonical = umi_ui_workspace_customisation_active_const(fresh);
    UMI_TEST_REQUIRE(canonical != NULL);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
        UMI_STUDIO_SURFACE_OUTLINE, true, false) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
        UMI_STUDIO_SURFACE_OBJECT_INSPECTOR, true, false) == UMI_STATUS_OK);
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model,
        new_id, "Current product layout") == UMI_STATUS_OK);
    layout = umi_ui_workspace_customisation_active_const(model);
    UMI_TEST_REQUIRE(layout != NULL && layout->locked && !model->edit_active);
    UMI_TEST_REQUIRE(strcmp(layout->layout_id, new_id) == 0 &&
        strcmp(layout->name, "Current product layout") == 0);
    UMI_TEST_REQUIRE(layout->window_count == canonical->window_count &&
        memcmp(layout->windows, canonical->windows, sizeof(layout->windows)) == 0);
    UMI_TEST_REQUIRE(model->layout_count == before->layout_count + 1U &&
        model->revision == before->revision + 2U);
    for (index = 0U; index < before->layout_count; ++index)
        UMI_TEST_REQUIRE(memcmp(&model->layouts[index], &before->layouts[index],
            sizeof(model->layouts[index])) == 0);
    UMI_TEST_REQUIRE(memcmp(&model->windows, &before->windows, sizeof(model->windows)) == 0);
    UMI_TEST_REQUIRE(memcmp(&model->groups, &before->groups, sizeof(model->groups)) == 0);
    UMI_TEST_REQUIRE(memcmp(&model->library, &before->library, sizeof(model->library)) == 0);
    UMI_TEST_REQUIRE(memcmp(&model->theme, &before->theme, sizeof(model->theme)) == 0);
    UMI_TEST_REQUIRE(memcmp(&model->edit_baseline, &before->edit_baseline, sizeof(model->edit_baseline)) == 0);
    UMI_TEST_REQUIRE(memcmp(&model->edit_groups_baseline, &before->edit_groups_baseline,
        sizeof(model->edit_groups_baseline)) == 0);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_select(model, "default") == UMI_STATUS_OK);
    layout = umi_ui_workspace_customisation_active_const(model);
    UMI_TEST_REQUIRE(umi_ui_workspace_layout_find_window(layout, "outline")->visible);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_select(model, new_id) == UMI_STATUS_OK);
    layout = umi_ui_workspace_customisation_active_const(model);
    UMI_TEST_REQUIRE(!umi_ui_workspace_layout_find_window(layout, "outline")->visible &&
        strcmp(umi_ui_workspace_layout_find_window(layout, "object-inspector")->placement_id,
            "auto-hide:left") == 0);

    /* User routing is global, not part of a geometry reset. Preserve a custom
     * destination-only group and an explicitly unlinked inspector exactly. */
    UMI_TEST_REQUIRE(umi_ui_window_group_define(&model->groups, "user.context.review",
        "orange", UMI_UI_WINDOW_CONTEXT_SYMBOL) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_window_group_assign(&model->groups, "user.context.review",
        "outline", UMI_UI_WINDOW_GROUP_DESTINATION) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_window_group_unassign(&model->groups, "object-inspector") == UMI_STATUS_OK);
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model,
        "umicom.studio.layout.product-default-2", "Keep my context routing") == UMI_STATUS_OK);
    layout = umi_ui_workspace_customisation_active_const(model);
    UMI_TEST_REQUIRE(layout != NULL && layout->window_count == canonical->window_count);
    UMI_TEST_REQUIRE(memcmp(&model->groups, &before->groups, sizeof(model->groups)) == 0);
    UMI_TEST_REQUIRE(strcmp(umi_ui_workspace_layout_find_window(layout, "outline")->context_group_id,
        "user.context.review") == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_layout_find_window(layout, "object-inspector")->context_group_id[0] == '\0');
    for (index = 0U; index < before->layout_count; ++index)
        UMI_TEST_REQUIRE(memcmp(&model->layouts[index], &before->layouts[index], sizeof(model->layouts[index])) == 0);
    for (index = 0U; index < layout->window_count; ++index) {
        UmiUiWorkspaceWindow expected = canonical->windows[index];
        /* All geometry/chrome still comes from the current default; only the
         * context link comes from the existing user's shared routing store. */
        (void)memcpy(expected.context_group_id, layout->windows[index].context_group_id,
            sizeof(expected.context_group_id));
        UMI_TEST_REQUIRE(memcmp(&expected, &layout->windows[index], sizeof(expected)) == 0);
    }

    /* Invalid names, identity collisions and active edits leave all records,
     * selection and transaction baselines unchanged. */
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model, new_id, "Replacement") == UMI_STATUS_ALREADY_EXISTS);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model, "umicom.trader.layout.new", "Foreign") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model, "umicom.studio.layout.", "Empty suffix") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model, "umicom.studio.layout.new", "") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model, "umicom.studio.layout.new", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    (void)memset(too_long, 'x', sizeof(too_long) - 1U);
    too_long[sizeof(too_long) - 1U] = '\0';
    (void)memcpy(too_long, UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX,
        sizeof(UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX) - 1U);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model, too_long, "Long ID") == UMI_STATUS_CAPACITY_EXCEEDED);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model,
        "umicom.studio.layout.product-default-3", "During edit") == UMI_STATUS_BUSY);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_cancel_edit(model) == UMI_STATUS_OK);

    /* Registrations are borrowed rather than replaced by a new seed. Missing
     * tools or malformed routing cannot publish a partial new layout. */
    *fresh = *model;
    model->windows.count -= 1U;
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model,
        "umicom.studio.layout.product-default-3", "Missing tool") == UMI_STATUS_NOT_FOUND);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    *model = *fresh;
    model->groups.count = UMI_UI_WINDOW_GROUP_MAX + 1U;
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model,
        "umicom.studio.layout.product-default-3", "Malformed groups") == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    *model = *fresh;
    (void)memset(model->groups.items[0].members[0].window_id, 'x',
        sizeof(model->groups.items[0].members[0].window_id));
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model,
        "umicom.studio.layout.product-default-3", "Unterminated member") == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    *model = *fresh;
    model->groups.count = 0U;
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model,
        "umicom.studio.layout.product-default-3", "No context mappings") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(memcmp(&model->groups, &before->groups, sizeof(model->groups)) == 0);
    layout = umi_ui_workspace_customisation_active_const(model);
    UMI_TEST_REQUIRE(layout != NULL);
    for (index = 0U; index < layout->window_count; ++index)
        UMI_TEST_REQUIRE(layout->windows[index].context_group_id[0] == '\0');
    *model = *fresh;
    model->revision = UINT64_MAX - 1U;
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model,
        "umicom.studio.layout.product-default-3", "Overflow") == UMI_STATUS_CAPACITY_EXCEEDED);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    *model = *fresh;
    while (model->layout_count < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS) {
        char id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
        int written = snprintf(id, sizeof(id), "umicom.studio.layout.product-default-%zu", model->layout_count);
        UMI_TEST_REQUIRE(written > 0 && (size_t)written < sizeof(id));
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model, id, "Another default") == UMI_STATUS_OK);
    }
    *before = *model;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_create_default(model,
        "umicom.studio.layout.no-room", "Full") == UMI_STATUS_CAPACITY_EXCEEDED);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    free(fresh); free(before); free(model);
    return EXIT_SUCCESS;
}

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

    /* Every real tool has a unique, reversible identity. Working presets keep
     * catalogue visibility; only the welcome preset collapses side tools. */
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
            const bool preset_visible = umi_studio_layout_preset_contains_surface(preset, binding->kind) != 0;
            const bool default_rail = strcmp(preset->preset_id,
                umi_studio_layout_preset_default()->preset_id) == 0 &&
                (binding->kind == UMI_STUDIO_SURFACE_OUTLINE || binding->kind == UMI_STUDIO_SURFACE_OBJECT_INSPECTOR ||
                 (preset_visible && (binding->region == UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR ||
                                     binding->region == UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR)));
            window = umi_ui_workspace_layout_find_window(saved, id);
            UMI_TEST_REQUIRE(window != NULL && strcmp(window->tool_id, id) == 0);
            UMI_TEST_REQUIRE(saved->window_count == umi_studio_surface_catalogue_count());
            UMI_TEST_REQUIRE(window->visible == (preset_visible && !default_rail));
            UMI_TEST_REQUIRE((strncmp(window->placement_id, "auto-hide:", sizeof("auto-hide:") - 1U) == 0) == default_rail);
        }
    }
    kind = UMI_STUDIO_SURFACE_EDITOR;
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_surface_kind("unknown", &kind) == UMI_STATUS_NOT_FOUND);
    UMI_TEST_REQUIRE(kind == UMI_STUDIO_SURFACE_EDITOR);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_surface_id((UmiStudioRuntimeSurfaceKind)0) == NULL);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_select(model, "default") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_runtime_workspace_select(model, "org.umicom.trader.default") == UMI_STATUS_INVALID_ARGUMENT);
    layout = umi_ui_workspace_customisation_active(model);
    /* Welcome side tools use real named edge rails, not empty permanently
     * occupied columns. Reveal/collapse remains transient adapter state. */
    {
        const UmiUiWorkspaceWindow *structure = umi_ui_workspace_layout_find_window(layout, "outline");
        const UmiUiWorkspaceWindow *inspector = umi_ui_workspace_layout_find_window(layout, "object-inspector");
        UmiUiWorkspaceWindow original;
        UMI_TEST_REQUIRE(structure != NULL && inspector != NULL);
        UMI_TEST_REQUIRE(strcmp(structure->title, "Structure") == 0 &&
            strcmp(inspector->title, "Object Inspector") == 0);
        UMI_TEST_REQUIRE(strcmp(structure->placement_id, "auto-hide:left") == 0 &&
            strcmp(inspector->placement_id, "auto-hide:left") == 0);
        UMI_TEST_REQUIRE(!structure->visible && !inspector->visible && !structure->pinned);
        original = *structure;
        *before = *model;
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
            UMI_STUDIO_SURFACE_OUTLINE, true, true) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
            UMI_STUDIO_SURFACE_OUTLINE, true, false) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(structure->visible && strcmp(structure->placement_id, "left") == 0 &&
            !structure->pinned && !model->edit_active && layout->locked);
        UMI_TEST_REQUIRE(structure->x == original.x && structure->y == original.y &&
            structure->width == original.width && structure->height == original.height &&
            strcmp(structure->context_group_id, original.context_group_id) == 0 &&
            strcmp(structure->stack_id, original.stack_id) == 0);
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
            UMI_STUDIO_SURFACE_OUTLINE, false, false) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(!structure->visible && strcmp(structure->placement_id, "left") == 0 &&
            layout->window_count == umi_studio_surface_catalogue_count());
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
            UMI_STUDIO_SURFACE_OUTLINE, true, false) == UMI_STATUS_OK && structure->visible);
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
            UMI_STUDIO_SURFACE_OUTLINE, true, true) == UMI_STATUS_OK && !structure->visible);

        /* Protected pinning remains an explicit geometry policy. Returning a
         * tool from a rail must never silently pin it or bypass a protection. */
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_pinned(model, "outline", true) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(model) == UMI_STATUS_OK);
        *before = *model;
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
            UMI_STUDIO_SURFACE_OUTLINE, true, false) == UMI_STATUS_PERMISSION_DENIED);
        UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_pinned(model, "outline", false) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_set_auto_hidden(model, "outline", false) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(!structure->pinned);
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_set_auto_hidden(model, "outline", true) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(!structure->pinned);
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(model) == UMI_STATUS_OK);

        /* The normal-mode helper joins an active edit, retaining its original
         * Cancel baseline and refusing revision wrap or protected Close. */
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
            UMI_STUDIO_SURFACE_OUTLINE, true, false) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(model->edit_active && !layout->locked && structure->visible);
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_cancel_edit(model) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(!structure->visible && strcmp(structure->placement_id, "auto-hide:left") == 0);
        umi_ui_workspace_layout_find_window_mutable(layout, "outline")->closable = false;
        *before = *model;
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
            UMI_STUDIO_SURFACE_OUTLINE, false, false) == UMI_STATUS_PERMISSION_DENIED);
        UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
        umi_ui_workspace_layout_find_window_mutable(layout, "outline")->closable = true;
        revision = model->revision; model->revision = UINT64_MAX;
        *before = *model;
        UMI_TEST_REQUIRE(umi_studio_runtime_workspace_set_tool_presentation(model,
            UMI_STUDIO_SURFACE_OUTLINE, true, false) == UMI_STATUS_CAPACITY_EXCEEDED);
        UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
        model->revision = revision;
        *before = *model;
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_set_tool_presentation(model,
            "unknown", true, false) == UMI_STATUS_NOT_FOUND);
        UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    }
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
    *before = *model;
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_set_tool_presentation(model,
        "editor", true, true) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
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
    UMI_TEST_REQUIRE(test_named_product_default() == EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
