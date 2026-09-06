/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/workspace_canvas.c
 *
 * PURPOSE:
 *   Adapt real Studio surfaces and presets to the common workspace transaction.
 *   Menus and renderers consume the same model instead of keeping panel arrays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/workspace_canvas.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/studio_runtime/layout_catalogue.h"

/* Validate before copying so a shortened ID can never select another tool. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *text)
{
    size_t length;
    if (destination == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, text, length + 1U);
    return UMI_STATUS_OK;
}

/* Only supported catalogue entries receive a usable singleton identity. */
const char *umi_studio_runtime_workspace_surface_id(UmiStudioRuntimeSurfaceKind kind)
{
    return umi_studio_surface_catalogue_for_kind(kind) == NULL ? NULL
        : umi_studio_runtime_surface_kind_text(kind);
}

/* Resolve public surface text without relying on shared component identities. */
UmiStatus umi_studio_runtime_workspace_surface_kind(
    const char *surface_id, UmiStudioRuntimeSurfaceKind *out_kind)
{
    size_t index;
    if (surface_id == NULL || out_kind == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < umi_studio_surface_catalogue_count(); ++index) {
        const UmiStudioRuntimeSurfaceBinding *binding = umi_studio_surface_catalogue_at(index);
        if (binding != NULL &&
            (strcmp(surface_id, umi_studio_runtime_surface_kind_text(binding->kind)) == 0 ||
             strcmp(surface_id, binding->ide_surface_id) == 0)) {
            *out_kind = binding->kind;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Native and product catalogues use one historical alias for the explorer.
 * Other surfaces match by exact identity; no unrelated component is substituted. */
static const UmiExperiencePanelDefinition *product_panel(UmiStudioRuntimeSurfaceKind kind)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.studio");
    const char *id = kind == UMI_STUDIO_SURFACE_EXPLORER ? "resource-explorer"
        : umi_studio_runtime_workspace_surface_id(kind);
    size_t index;
    if (experience == NULL || id == NULL) return NULL;
    for (index = 0U; index < experience->panel_count; ++index)
        if (strcmp(id, experience->panels[index].panel_id) == 0)
            return &experience->panels[index];
    return NULL;
}

/* Native semantic regions become the portable workspace's dock regions. */
static UmiUiPlacement surface_placement(const UmiStudioRuntimeSurfaceBinding *binding)
{
    switch (binding->region) {
        case UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR: return UMI_UI_PLACEMENT_LEFT;
        case UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR: return UMI_UI_PLACEMENT_RIGHT;
        case UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL: return UMI_UI_PLACEMENT_BOTTOM;
        default: return UMI_UI_PLACEMENT_CENTRE;
    }
}

/* Preserve the native tool groups while respecting product metadata that
 * explicitly omits context linking for a particular surface. */
static const char *surface_context(UmiStudioRuntimeSurfaceKind kind)
{
    const UmiExperiencePanelDefinition *panel = product_panel(kind);
    if (panel != NULL && (panel->flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) == 0U) return "";
    if (kind == UMI_STUDIO_SURFACE_TEST_EXPLORER) return "studio.context.testing";
    if (kind == UMI_STUDIO_SURFACE_LEARNING || kind == UMI_STUDIO_SURFACE_DOCUMENTATION) return "studio.context.learning";
    if (kind >= UMI_STUDIO_SURFACE_VISUAL_DESIGNER && kind <= UMI_STUDIO_SURFACE_DESIGN_PREVIEW) return "studio.context.visual-design";
    if (kind >= UMI_STUDIO_SURFACE_AI_CHAT && kind <= UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON) return "studio.context.ai";
    return "studio.context.development";
}

/* Construct a plain model record. Widget ownership stays in the presentation
 * adapter and document ownership stays in the existing document coordinator. */
static UmiStatus surface_window(
    const UmiStudioRuntimeSurfaceBinding *binding, bool visible,
    UmiUiWorkspaceWindow *window)
{
    const char *id = umi_studio_runtime_workspace_surface_id(binding->kind);
    const char *placement = umi_ui_placement_text(surface_placement(binding));
    UmiApplicationSuiteLayoutRect rect =
        umi_application_suite_layout_region_rect(surface_placement(binding));
    UmiStatus status;
    const UmiExperiencePanelDefinition *panel = product_panel(binding->kind);
    if (panel != NULL && (panel->flags & UMI_EXPERIENCE_PANEL_DOCKABLE) == 0U)
        return UMI_STATUS_PERMISSION_DENIED;
    (void)memset(window, 0, sizeof(*window));
    status = copy_text(window->window_id, sizeof(window->window_id), id);
    if (status == UMI_STATUS_OK) status = copy_text(window->tool_id, sizeof(window->tool_id), id);
    if (status == UMI_STATUS_OK) status = copy_text(window->title, sizeof(window->title), binding->title);
    if (status == UMI_STATUS_OK) status = copy_text(window->placement_id, sizeof(window->placement_id), placement);
    if (status == UMI_STATUS_OK) status = copy_text(window->stack_id, sizeof(window->stack_id), placement);
    if (status == UMI_STATUS_OK) status = copy_text(window->group_id, sizeof(window->group_id), placement);
    if (status == UMI_STATUS_OK) status = copy_text(window->context_group_id, sizeof(window->context_group_id), surface_context(binding->kind));
    window->x = rect.x; window->y = rect.y;
    window->width = rect.width; window->height = rect.height;
    window->visible = visible;
    window->closable = true;
    window->resizable = true;
    window->z_order = binding->order;
    return status;
}

/* Seed only real native surfaces, leaving room for additional user layouts. */
UmiStatus umi_studio_runtime_workspace_seed(UmiUiWorkspaceCustomisation *model)
{
    UmiUiWorkspaceCustomisation *candidate;
    UmiUiWorkspaceLayout *layout;
    UmiStatus status = UMI_STATUS_OK;
    size_t index, preset_index;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->edit_active) return UMI_STATUS_BUSY;
    if (model->layout_count != 0U || model->windows.count != 0U)
        return UMI_STATUS_ALREADY_EXISTS;
    if (umi_studio_layout_catalogue_count() >= UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS ||
        umi_studio_surface_catalogue_count() > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate = (UmiUiWorkspaceCustomisation *)calloc(1U, sizeof(*candidate));
    layout = (UmiUiWorkspaceLayout *)calloc(1U, sizeof(*layout));
    if (candidate == NULL || layout == NULL) {
        free(candidate); free(layout);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_ui_workspace_customisation_init(candidate);
    candidate->theme = model->theme;
    status = umi_ui_window_group_define(&candidate->groups, "studio.context.development", "blue", UMI_UI_WINDOW_CONTEXT_PROJECT);
    if (status == UMI_STATUS_OK) status = umi_ui_window_group_define(&candidate->groups, "studio.context.testing", "green", UMI_UI_WINDOW_CONTEXT_RUN);
    if (status == UMI_STATUS_OK) status = umi_ui_window_group_define(&candidate->groups, "studio.context.learning", "yellow", UMI_UI_WINDOW_CONTEXT_DOCUMENT);
    if (status == UMI_STATUS_OK) status = umi_ui_window_group_define(&candidate->groups, "studio.context.visual-design", "purple", UMI_UI_WINDOW_CONTEXT_DOCUMENT);
    if (status == UMI_STATUS_OK) status = umi_ui_window_group_define(&candidate->groups, "studio.context.ai", "purple", UMI_UI_WINDOW_CONTEXT_PROJECT);
    for (index = 0U; status == UMI_STATUS_OK && index < umi_studio_surface_catalogue_count(); ++index) {
        const UmiStudioRuntimeSurfaceBinding *binding = umi_studio_surface_catalogue_at(index);
        UmiUiWindowDescriptor descriptor;
        status = umi_studio_runtime_surface_binding_validate(binding);
        if (status != UMI_STATUS_OK) break;
        (void)memset(&descriptor, 0, sizeof(descriptor));
        status = copy_text(descriptor.tool_id, sizeof(descriptor.tool_id),
            umi_studio_runtime_workspace_surface_id(binding->kind));
        if (status == UMI_STATUS_OK) status = copy_text(descriptor.title, sizeof(descriptor.title), binding->title);
        if (status == UMI_STATUS_OK) status = copy_text(descriptor.description, sizeof(descriptor.description), binding->title);
        if (status == UMI_STATUS_OK) status = copy_text(descriptor.icon_name, sizeof(descriptor.icon_name), "application-panel");
        descriptor.category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
        descriptor.supports_multiple = false;
        descriptor.default_width = 0.48;
        descriptor.default_height = 0.52;
        if (status == UMI_STATUS_OK) status = umi_ui_window_catalogue_register(&candidate->windows, &descriptor);
        if (status == UMI_STATUS_OK && surface_context(binding->kind)[0] != '\0')
            status = umi_ui_window_group_assign(&candidate->groups, surface_context(binding->kind),
                descriptor.tool_id, UMI_UI_WINDOW_GROUP_BIDIRECTIONAL);
    }
    /* Existing native presets define exactly which real tools start visible.
     * Hidden singleton records keep their last placement for later reopening. */
    for (preset_index = 0U; status == UMI_STATUS_OK && preset_index < umi_studio_layout_catalogue_count(); ++preset_index) {
        const UmiStudioRuntimeLayoutPresetDefinition *preset = umi_studio_layout_catalogue_at(preset_index);
        status = umi_studio_layout_preset_validate(preset);
        if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_init(layout, preset->preset_id, preset->title);
        if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout, false);
        for (index = 0U; status == UMI_STATUS_OK && index < umi_studio_surface_catalogue_count(); ++index) {
            const UmiStudioRuntimeSurfaceBinding *binding = umi_studio_surface_catalogue_at(index);
            UmiUiWorkspaceWindow window;
            status = surface_window(binding,
                umi_studio_layout_preset_contains_surface(preset, binding->kind) != 0, &window);
            if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_add_window(layout, &window);
        }
        if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout, true);
        if (status == UMI_STATUS_OK) status = umi_ui_workspace_customisation_add_layout(candidate, layout);
    }
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_customisation_activate(candidate,
        umi_studio_layout_preset_default()->preset_id);
    if (status == UMI_STATUS_OK) *model = *candidate;
    free(layout); free(candidate);
    return status;
}

/* A complete prefix and nonempty suffix keep layouts inside this product. */
static bool qualified_id(const char *id)
{
    const size_t length = sizeof(UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX) - 1U;
    return id != NULL && strncmp(id, UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX, length) == 0 && id[length] != '\0';
}

/* Accept existing short menu IDs without changing stored qualified identities. */
UmiStatus umi_studio_runtime_workspace_select(UmiUiWorkspaceCustomisation *model, const char *layout_id)
{
    char qualified[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    const size_t prefix_length = sizeof(UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX) - 1U;
    size_t length;
    if (model == NULL || layout_id == NULL || layout_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (qualified_id(layout_id)) return umi_ui_workspace_customisation_activate(model, layout_id);
    /* A foreign qualified name must not accidentally become a local suffix. */
    if (strchr(layout_id, '.') != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(layout_id);
    if (length >= sizeof(qualified) - prefix_length) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(qualified, UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX, prefix_length);
    (void)memcpy(qualified + prefix_length, layout_id, length + 1U);
    return umi_ui_workspace_customisation_activate(model, qualified);
}

/* Create and unlock an empty canvas as one rollback-safe model operation. */
UmiStatus umi_studio_runtime_workspace_create_blank(
    UmiUiWorkspaceCustomisation *model, const char *layout_id, const char *name)
{
    UmiUiWorkspaceCustomisation *candidate;
    UmiStatus status;
    if (model == NULL || !qualified_id(layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Creation and unlocking are one request: even a failed begin-edit must
     * not leave the user switched away from their previous layout. */
    candidate = (UmiUiWorkspaceCustomisation *)malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *model;
    status = umi_ui_workspace_customisation_create_blank_layout(candidate, layout_id, name);
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_customisation_begin_edit(candidate);
    if (status == UMI_STATUS_OK) *model = *candidate;
    free(candidate);
    return status;
}

/* Opening a tool may use a short transaction while the layout is locked.
 * Hiding is a structural removal and must respect the explicit edit session,
 * layout lock, pinning and closable policy before any candidate is created. */
UmiStatus umi_studio_runtime_workspace_set_visible(
    UmiUiWorkspaceCustomisation *model, UmiStudioRuntimeSurfaceKind kind, bool visible)
{
    const UmiStudioRuntimeSurfaceBinding *binding = umi_studio_surface_catalogue_for_kind(kind);
    const char *id = umi_studio_runtime_workspace_surface_id(kind);
    UmiUiWorkspaceCustomisation *candidate;
    UmiUiWorkspaceLayout *layout;
    const UmiUiWorkspaceWindow *window;
    UmiStatus status = UMI_STATUS_OK;
    bool automatic_edit;
    const UmiExperiencePanelDefinition *panel;
    if (model == NULL || binding == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(model);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    window = umi_ui_workspace_layout_find_window(layout, id);
    if (window != NULL && strcmp(window->tool_id, id) != 0) return UMI_STATUS_INVALID_STATE;
    panel = product_panel(kind);
    if (visible && panel != NULL) {
        uint32_t required = window != NULL && window->floating
            ? UMI_EXPERIENCE_PANEL_FLOATABLE : UMI_EXPERIENCE_PANEL_DOCKABLE;
        if ((panel->flags & required) == 0U) return UMI_STATUS_PERMISSION_DENIED;
    }
    if (!visible) {
        if (!model->edit_active) return UMI_STATUS_INVALID_STATE;
        if (layout->locked || (window != NULL && (window->pinned || !window->closable)))
            return UMI_STATUS_PERMISSION_DENIED;
    }
    if ((window == NULL && !visible) || (window != NULL && window->visible == visible)) return UMI_STATUS_OK;
    if (model->revision > UINT64_MAX - 8U || layout->revision > UINT64_MAX - 8U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate = (UmiUiWorkspaceCustomisation *)malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *model;
    automatic_edit = !candidate->edit_active;
    if (automatic_edit) status = umi_ui_workspace_customisation_begin_edit(candidate);
    layout = umi_ui_workspace_customisation_active(candidate);
    if (status == UMI_STATUS_OK && window == NULL) {
        char opened_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
        status = umi_ui_workspace_customisation_open_window(candidate, id,
            /* Blank canvases open tools as independent cards, while native
             * presets keep the surface's normal dock region. */
            umi_studio_layout_catalogue_find(layout->layout_id) == NULL
                ? UMI_UI_WORKSPACE_CANVAS_PLACEMENT
                : umi_ui_placement_text(surface_placement(binding)),
            false, 0U, opened_id, sizeof(opened_id));
        if (status == UMI_STATUS_OK && surface_context(kind)[0] != '\0')
            status = umi_ui_workspace_customisation_assign_context_group(candidate,
                opened_id, surface_context(kind), UMI_UI_WINDOW_GROUP_BIDIRECTIONAL);
    } else if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_visible(layout, id, visible);
        if (status == UMI_STATUS_OK) candidate->revision += 1U;
    }
    if (status == UMI_STATUS_OK && automatic_edit) status = umi_ui_workspace_customisation_commit_edit(candidate);
    if (status == UMI_STATUS_OK) *model = *candidate;
    free(candidate);
    return status;
}

/* Match the live singleton before applying portable product capability flags.
 * Tools not yet in the broader experience catalogue keep their native binding
 * support, but an unregistered/foreign window is never treated as a real tool. */
static UmiStatus settings_policy(
    const UmiUiWorkspaceCustomisation *model, const UmiUiWorkspacePanelSettings *settings)
{
    const UmiUiWorkspaceLayout *layout = umi_ui_workspace_customisation_active_const(model);
    const UmiUiWorkspaceWindow *window;
    const UmiExperiencePanelDefinition *panel;
    UmiStudioRuntimeSurfaceKind kind;
    UmiStatus status;
    if (model == NULL || settings == NULL || settings->window_id == NULL || settings->placement_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    window = umi_ui_workspace_layout_find_window(layout, settings->window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_studio_runtime_workspace_surface_kind(window->tool_id, &kind);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(window->window_id, umi_studio_runtime_workspace_surface_id(kind)) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    panel = product_panel(kind);
    if (panel != NULL) {
        uint32_t required = settings->floating ? UMI_EXPERIENCE_PANEL_FLOATABLE : UMI_EXPERIENCE_PANEL_DOCKABLE;
        if (settings->auto_hidden) required |= UMI_EXPERIENCE_PANEL_AUTO_HIDE;
        if (settings->context_group_id != NULL && settings->context_group_id[0] != '\0') required |= UMI_EXPERIENCE_PANEL_CONTEXT_LINKED;
        if ((panel->flags & required) != required) return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}

/* Capability validation precedes the shared atomic placement transaction. */
UmiStatus umi_studio_runtime_workspace_apply_panel_settings(
    UmiUiWorkspaceCustomisation *model, const UmiUiWorkspacePanelSettings *settings)
{
    UmiStatus status = settings_policy(model, settings);
    return status != UMI_STATUS_OK ? status : umi_ui_workspace_customisation_apply_panel_settings(model, settings);
}

/* A pointer gesture may arrive after selection changed; reject that stale
 * revision before looking up a panel or changing any saved geometry. */
UmiStatus umi_studio_runtime_workspace_place_canvas(
    UmiUiWorkspaceCustomisation *model, const char *window_id,
    const UmiApplicationSuiteLayoutRect *rect, uint64_t expected_layout_revision)
{
    const UmiUiWorkspaceLayout *layout;
    UmiUiWorkspacePanelSettings settings;
    UmiStatus status;
    if (model == NULL || window_id == NULL || rect == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active_const(model);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    if (layout->revision != expected_layout_revision) return UMI_STATUS_INVALID_STATE;
    settings = umi_ui_workspace_panel_settings_default(window_id);
    settings.placement_id = UMI_UI_WORKSPACE_CANVAS_PLACEMENT;
    status = settings_policy(model, &settings);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_workspace_customisation_place_canvas_window(model, window_id,
        rect->x, rect->y, rect->width, rect->height);
}

/* These flags are stored on the existing window record, not in GTK state. */
static UmiStatus set_window_flag(
    UmiUiWorkspaceCustomisation *model, const char *window_id,
    bool value, bool pin_control)
{
    UmiUiWorkspaceLayout *layout;
    const UmiUiWorkspaceWindow *window;
    UmiStudioRuntimeSurfaceKind kind;
    UmiStatus status;
    if (model == NULL || window_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!model->edit_active) return UMI_STATUS_INVALID_STATE;
    layout = umi_ui_workspace_customisation_active(model);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = umi_ui_workspace_layout_find_window(layout, window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_studio_runtime_workspace_surface_kind(window->tool_id, &kind);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(window_id, umi_studio_runtime_workspace_surface_id(kind)) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if ((pin_control ? window->pinned : window->maximised) == value) return UMI_STATUS_OK;
    if (!pin_control && (window->pinned || !window->resizable)) return UMI_STATUS_PERMISSION_DENIED;
    if (model->revision == UINT64_MAX || layout->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = pin_control ? umi_ui_workspace_layout_set_pinned(layout, window_id, value)
        : umi_ui_workspace_layout_set_maximised(layout, window_id, value);
    if (status == UMI_STATUS_OK) model->revision += 1U;
    return status;
}

/* Pinning is a property of the shared instance, not its current GTK frame. */
UmiStatus umi_studio_runtime_workspace_set_pinned(
    UmiUiWorkspaceCustomisation *model, const char *window_id, bool pinned)
{
    return set_window_flag(model, window_id, pinned, true);
}

/* Keep the original rectangle available for restoring from maximised view. */
UmiStatus umi_studio_runtime_workspace_set_maximised(
    UmiUiWorkspaceCustomisation *model, const char *window_id, bool maximised)
{
    return set_window_flag(model, window_id, maximised, false);
}

/* Only semantic regions are projected. Pixel and normalized geometry remain
 * in the shared workspace model read by the actual presentation adapter. */
static UmiApplicationShellRegion projected_region(const UmiUiWorkspaceWindow *window)
{
    if (window->floating) return UMI_APPLICATION_SHELL_REGION_WINDOW;
    if (strcmp(window->placement_id, "left") == 0) return UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR;
    if (strcmp(window->placement_id, "right") == 0) return UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR;
    if (strcmp(window->placement_id, "bottom") == 0) return UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL;
    return UMI_APPLICATION_SHELL_REGION_EDITOR;
}

/* Publish a compatibility view only after all native identities can resolve. */
UmiStatus umi_studio_runtime_workspace_project_shell(
    const UmiUiWorkspaceCustomisation *model, UmiApplicationShellRegistry *registry,
    UmiApplicationShellState *shell_state, UmiApplicationShellLayout *shell_layout)
{
    const UmiUiWorkspaceLayout *active;
    UmiApplicationShellContribution *contributions;
    UmiApplicationShellLayout *candidate_layout;
    UmiApplicationShellState candidate_state;
    const size_t count = umi_studio_surface_catalogue_count();
    size_t index, other, chosen = count, editor = count, first = count;
    UmiStatus status = UMI_STATUS_OK;
    if (model == NULL || registry == NULL || shell_state == NULL || shell_layout == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    active = umi_ui_workspace_customisation_active_const(model);
    if (active == NULL) return UMI_STATUS_NOT_FOUND;
    if (active->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS ||
        shell_layout->placement_count > UMI_APPLICATION_SHELL_LAYOUT_MAX_ITEMS)
        return UMI_STATUS_INVALID_STATE;
    if (count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS || shell_state->revision == UINT64_MAX ||
        shell_layout->revision > UINT64_MAX - (uint64_t)count ||
        umi_application_shell_registry_revision(registry) > UINT64_MAX - (uint64_t)count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    contributions = (UmiApplicationShellContribution *)calloc(count, sizeof(*contributions));
    candidate_layout = (UmiApplicationShellLayout *)malloc(sizeof(*candidate_layout));
    if (contributions == NULL || candidate_layout == NULL) {
        free(contributions); free(candidate_layout);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *candidate_layout = *shell_layout;
    candidate_state = *shell_state;
    candidate_state.primary_sidebar_visible = 0;
    candidate_state.secondary_sidebar_visible = 0;
    candidate_state.bottom_panel_visible = 0;
    status = copy_text(candidate_layout->layout_id, sizeof(candidate_layout->layout_id), active->layout_id);
    if (status == UMI_STATUS_OK) status = copy_text(candidate_layout->title, sizeof(candidate_layout->title), active->name);
    /* Resolve every binding and stage every layout change before changing the
     * registry. Under the same-thread contract, these validated entries cannot
     * disappear between this preflight and the non-allocating state updates. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiStudioRuntimeSurfaceBinding *binding = umi_studio_surface_catalogue_at(index);
        const UmiUiWorkspaceWindow *window = umi_ui_workspace_layout_find_window(active,
            umi_studio_runtime_workspace_surface_id(binding->kind));
        UmiApplicationShellRegion region = window == NULL ? binding->region : projected_region(window);
        const bool visible = window != NULL && window->visible;
        status = umi_studio_runtime_surface_resolve(registry, binding, &contributions[index]);
        if (status != UMI_STATUS_OK) break;
        if (contributions[index].revision == UINT64_MAX) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
        for (other = 0U; other < index; ++other) {
            if (strcmp(contributions[index].contribution_id, contributions[other].contribution_id) == 0)
                status = UMI_STATUS_INVALID_STATE;
        }
        if (status != UMI_STATUS_OK) break;
        if (visible) contributions[index].flags |= UMI_APPLICATION_SHELL_VISIBLE;
        else contributions[index].flags &= ~(UmiApplicationShellFlags)UMI_APPLICATION_SHELL_VISIBLE;
        status = umi_application_shell_layout_place(candidate_layout,
            contributions[index].contribution_id,
            window != NULL ? window->stack_id : binding->container_id,
            region, (size_t)(binding->order < 0 ? 0 : binding->order), visible ? 1 : 0);
        if (visible) {
            if (first == count) first = index;
            if (binding->kind == UMI_STUDIO_SURFACE_EDITOR) editor = index;
            if (strcmp(shell_state->active_view_id, contributions[index].contribution_id) == 0) chosen = index;
            if (region == UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR) candidate_state.primary_sidebar_visible = 1;
            if (region == UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR) candidate_state.secondary_sidebar_visible = 1;
            if (region == UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL) candidate_state.bottom_panel_visible = 1;
        }
    }
    if (status == UMI_STATUS_OK) {
        const UmiStudioRuntimeLayoutPresetDefinition *preset = umi_studio_layout_catalogue_find(active->layout_id);
        if (chosen == count) chosen = editor != count ? editor : first;
        /* Empty canvases deliberately have no active or focused tool. */
        candidate_state.active_view_id[0] = '\0';
        candidate_state.focused_view_id[0] = '\0';
        candidate_state.active_activity_id[0] = '\0';
        if (chosen != count) {
            const UmiStudioRuntimeSurfaceBinding *binding = umi_studio_surface_catalogue_at(chosen);
            status = copy_text(candidate_state.active_view_id, sizeof(candidate_state.active_view_id), contributions[chosen].contribution_id);
            if (status == UMI_STATUS_OK) status = copy_text(candidate_state.focused_view_id, sizeof(candidate_state.focused_view_id), contributions[chosen].contribution_id);
            if (status == UMI_STATUS_OK) status = copy_text(candidate_state.active_activity_id, sizeof(candidate_state.active_activity_id), binding->activity_id != NULL ? binding->activity_id : "");
        }
        if (status == UMI_STATUS_OK) status = copy_text(candidate_state.active_perspective_id, sizeof(candidate_state.active_perspective_id),
            preset != NULL ? preset->perspective_id : active->layout_id);
        candidate_state.zen_mode = preset != NULL ? preset->zen_mode : 0;
        if (preset != NULL) candidate_state.status_bar_visible = preset->status_bar_visible;
        candidate_state.revision += 1U;
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiApplicationShellContribution *entry = &contributions[index];
        status = umi_application_shell_registry_set_state(registry, entry->contribution_id,
            (entry->flags & UMI_APPLICATION_SHELL_VISIBLE) != 0U,
            (entry->flags & UMI_APPLICATION_SHELL_ENABLED) != 0U,
            (entry->flags & UMI_APPLICATION_SHELL_CHECKED) != 0U, entry->badge_count);
    }
    if (status == UMI_STATUS_OK) {
        *shell_layout = *candidate_layout;
        *shell_state = candidate_state;
    }
    free(candidate_layout); free(contributions);
    return status;
}
