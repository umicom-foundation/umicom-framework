/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_customisation.c
 *
 * PURPOSE:
 *   Implement the workspace customisation behavior for
 *   Umicom Framework.
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

#include "umicom/ui/workspace_geometry.h"

#define UMI_UI_AUTO_HIDE_PREFIX "auto-hide:"

/* Copy user-visible metadata without allowing truncation to create ambiguous
 * identifiers.  The destination remains terminated whenever copying works. */
static UmiStatus customisation_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/*
 * Initialise ui workspace customisation from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_workspace_customisation_init(UmiUiWorkspaceCustomisation *customisation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL) return;
    (void)memset(customisation, 0, sizeof(*customisation));
    umi_ui_window_catalogue_init(&customisation->windows);
    (void)umi_ui_theme_profile_init(
        &customisation->theme,
        "umicom-dark",
        "Umicom Dark",
        UMI_UI_THEME_MODE_DARK,
        UMI_UI_DENSITY_COMFORTABLE);
    customisation->revision = 1U;
}

/* Provide the layout index operation used by this module and its client applications. */
static size_t layout_index(
    const UmiUiWorkspaceCustomisation *customisation,
    const char *layout_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || layout_id == NULL)
        return UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < customisation->layout_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(customisation->layouts[index].layout_id, layout_id) == 0)
            return index;
    }
    return UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS;
}

/*
 * Provide the ui workspace customisation add layout operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_add_layout(UmiUiWorkspaceCustomisation *customisation,const UmiUiWorkspaceLayout *layout)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || layout == NULL || layout->layout_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (customisation->edit_active) return UMI_STATUS_BUSY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < customisation->layout_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(customisation->layouts[index].layout_id,layout->layout_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (customisation->layout_count >= UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    customisation->layouts[customisation->layout_count++] = *layout; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (customisation->active_layout_id[0] == '\0') (void)snprintf(customisation->active_layout_id,sizeof(customisation->active_layout_id),"%s",layout->layout_id); customisation->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace customisation activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_activate(UmiUiWorkspaceCustomisation *customisation,const char *layout_id)
{
    size_t index;
    int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (customisation->edit_active) return UMI_STATUS_BUSY;
    index = layout_index(customisation, layout_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS)
        return UMI_STATUS_NOT_FOUND;
    length = snprintf(
        customisation->active_layout_id,
        sizeof(customisation->active_layout_id),
        "%s",
        layout_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0 ||
        (size_t)length >= sizeof(customisation->active_layout_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    customisation->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace customisation active operation used by this module and its
 * client applications.
 */
UmiUiWorkspaceLayout *umi_ui_workspace_customisation_active(UmiUiWorkspaceCustomisation *customisation)
{
    size_t index = layout_index(
        customisation,
        customisation != NULL ? customisation->active_layout_id : NULL);
    return index < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS
        ? &customisation->layouts[index]
        : NULL;
}

/*
 * Provide the ui workspace customisation active const operation used by this module and
 * its client applications.
 */
const UmiUiWorkspaceLayout *umi_ui_workspace_customisation_active_const(
    const UmiUiWorkspaceCustomisation *customisation)
{
    size_t index = layout_index(
        customisation,
        customisation != NULL ? customisation->active_layout_id : NULL);
    return index < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS
        ? &customisation->layouts[index]
        : NULL;
}

/*
 * Provide the ui workspace customisation clone layout operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workspace_customisation_clone_layout(
    UmiUiWorkspaceCustomisation *customisation,
    const char *source_layout_id,
    const char *layout_id_value,
    const char *name)
{
    UmiUiWorkspaceLayout clone;
    size_t source_index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || source_layout_id == NULL ||
        layout_id_value == NULL || name == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (customisation->edit_active) return UMI_STATUS_BUSY;
    source_index = layout_index(customisation, source_layout_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (source_index == UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS)
        return UMI_STATUS_NOT_FOUND;
    status = umi_ui_workspace_layout_clone(
        &customisation->layouts[source_index],
        layout_id_value,
        name,
        &clone);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ui_workspace_customisation_add_layout(customisation, &clone);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ui_workspace_customisation_activate(
            customisation, layout_id_value);
    return status;
}

/*
 * Provide the ui workspace customisation remove layout operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workspace_customisation_remove_layout(
    UmiUiWorkspaceCustomisation *customisation,
    const char *layout_id_value)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || layout_id_value == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (customisation->edit_active) return UMI_STATUS_BUSY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (customisation->layout_count <= 1U) return UMI_STATUS_INVALID_STATE;
    index = layout_index(customisation, layout_id_value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS)
        return UMI_STATUS_NOT_FOUND;
    (void)memmove(
        &customisation->layouts[index],
        &customisation->layouts[index + 1U],
        (customisation->layout_count - index - 1U) *
            sizeof(customisation->layouts[0]));
    customisation->layout_count -= 1U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(customisation->active_layout_id, layout_id_value) == 0) {
        (void)snprintf(
            customisation->active_layout_id,
            sizeof(customisation->active_layout_id),
            "%s",
            customisation->layouts[0].layout_id);
    }
    customisation->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui workspace customisation rename active operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workspace_customisation_rename_active(
    UmiUiWorkspaceCustomisation *customisation,
    const char *name)
{
    UmiUiWorkspaceLayout *active =
        umi_ui_workspace_customisation_active(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (active == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!customisation->edit_active) return UMI_STATUS_INVALID_STATE;
    return umi_ui_workspace_layout_rename(active, name);
}

/*
 * Provide the ui workspace customisation begin edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_begin_edit(
    UmiUiWorkspaceCustomisation *customisation)
{
    UmiUiWorkspaceLayout *active;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (customisation->edit_active) return UMI_STATUS_BUSY;
    active = umi_ui_workspace_customisation_active(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (active == NULL) return UMI_STATUS_NOT_FOUND;
    /* Capture both halves of the editable workspace. A panel stores its group
     * identifier in the layout, while the group store keeps the reverse member
     * list used to route context. Restoring only one half leaves stale links. */
    customisation->edit_baseline = *active;
    customisation->edit_groups_baseline = customisation->groups;
    customisation->edit_started_revision = active->revision;
    status = umi_ui_workspace_layout_set_locked(active, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    customisation->edit_active = true;
    customisation->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui workspace customisation commit edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_commit_edit(
    UmiUiWorkspaceCustomisation *customisation)
{
    UmiUiWorkspaceLayout *active;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!customisation->edit_active) return UMI_STATUS_INVALID_STATE;
    active = umi_ui_workspace_customisation_active(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (active == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ui_workspace_layout_set_locked(active, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&customisation->edit_baseline, 0,
                 sizeof(customisation->edit_baseline));
    (void)memset(&customisation->edit_groups_baseline, 0,
                 sizeof(customisation->edit_groups_baseline));
    customisation->edit_active = false;
    customisation->edit_started_revision = 0U;
    customisation->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui workspace customisation cancel edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_cancel_edit(
    UmiUiWorkspaceCustomisation *customisation)
{
    UmiUiWorkspaceLayout *active;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!customisation->edit_active) return UMI_STATUS_INVALID_STATE;
    active = umi_ui_workspace_customisation_active(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (active == NULL) return UMI_STATUS_NOT_FOUND;
    /* Restore the geometry and the reverse context membership together so a
     * cancelled edit cannot continue routing events through an abandoned link. */
    *active = customisation->edit_baseline;
    customisation->groups = customisation->edit_groups_baseline;
    (void)memset(&customisation->edit_baseline, 0,
                 sizeof(customisation->edit_baseline));
    (void)memset(&customisation->edit_groups_baseline, 0,
                 sizeof(customisation->edit_groups_baseline));
    customisation->edit_active = false;
    customisation->edit_started_revision = 0U;
    customisation->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui workspace customisation open window operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_open_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *tool_id,
    const char *group_id,
    bool floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity)
{
    const UmiUiWindowDescriptor *descriptor;
    UmiUiWorkspaceLayout *active;
    UmiUiWorkspaceWindow window;
    UmiUiWorkspaceLayout before;
    size_t instance_count;
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || tool_id == NULL || group_id == NULL ||
        out_window_id == NULL || out_window_id_capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!customisation->edit_active) return UMI_STATUS_INVALID_STATE;
    active = umi_ui_workspace_customisation_active(customisation);
    descriptor = umi_ui_window_catalogue_find(&customisation->windows, tool_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (active == NULL || descriptor == NULL) return UMI_STATUS_NOT_FOUND;
    instance_count = umi_ui_workspace_layout_count_tool(active, tool_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!descriptor->supports_multiple && instance_count != 0U)
        return UMI_STATUS_ALREADY_EXISTS;

    (void)memset(&window, 0, sizeof(window));
    written = snprintf(
        window.window_id,
        sizeof(window.window_id),
        instance_count == 0U ? "%s" : "%s-%zu",
        tool_id,
        instance_count + 1U);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(window.window_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(window.title, sizeof(window.title), "%s", descriptor->title);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(window.title))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(window.tool_id, sizeof(window.tool_id), "%s", tool_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(window.tool_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(window.group_id, sizeof(window.group_id), "%s", group_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(window.group_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(window.stack_id, sizeof(window.stack_id), "%s", group_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(window.stack_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(
        out_window_id,
        out_window_id_capacity,
        "%s",
        window.window_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= out_window_id_capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    window.width = descriptor->default_width > 0.0
        ? descriptor->default_width
        : 0.40;
    window.height = descriptor->default_height > 0.0
        ? descriptor->default_height
        : 0.50;
    /* Apply this branch only when its contract condition is satisfied. */
    if (window.width > 1.0) window.width = 1.0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (window.height > 1.0) window.height = 1.0;
    window.x = (1.0 - window.width) / 2.0;
    window.y = (1.0 - window.height) / 2.0;
    {
        UmiUiPlacement placement;
        /* Apply this branch only when its contract condition is satisfied. */
        if (floating) {
            placement = UMI_UI_PLACEMENT_FLOATING;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (umi_ui_placement_parse(group_id, &placement) !=
                   UMI_STATUS_OK) {
            placement = UMI_UI_PLACEMENT_CENTRE;
        }
        {
            UmiUiWorkspaceRect region =
                umi_ui_workspace_region_rect(placement);
            written = snprintf(window.placement_id,
                               sizeof(window.placement_id), "%s",
                               umi_ui_placement_text(placement));
            /* Apply this branch only when its contract condition is satisfied. */
            if (written < 0 ||
                (size_t)written >= sizeof(window.placement_id))
                return UMI_STATUS_CAPACITY_EXCEEDED;
            window.x = region.x;
            window.y = region.y;
            window.width = region.width;
            window.height = region.height;
        }
    }
    window.visible = true;
    window.floating = floating;
    window.closable = true;
    window.resizable = true;
    window.z_order = (int32_t)active->window_count;

    /* Opening is transactional: a catalogue failure must not leave a window
     * in the layout without matching recent-window metadata. */
    before = *active;
    status = umi_ui_workspace_layout_add_window(active, &window);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_window_catalogue_record_open(
            &customisation->windows, tool_id, opened_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        *active = before;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) customisation->revision += 1U;
    return status;
}

/* Restore the entire active layout when a multi-step mutation fails. */
static UmiStatus finish_layout_mutation(
    UmiUiWorkspaceCustomisation *customisation,
    UmiUiWorkspaceLayout *layout,
    const UmiUiWorkspaceLayout *before,
    UmiStatus status)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        *layout = *before;
        return status;
    }
    customisation->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui workspace customisation dock window operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_dock_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    const char *placement_id,
    const char *stack_id)
{
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceLayout before;
    UmiUiWorkspaceWindow *window;
    UmiUiWorkspaceRect bounds;
    UmiUiPlacement placement;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || window_id == NULL ||
        placement_id == NULL || stack_id == NULL ||
        placement_id[0] == '\0' || stack_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!customisation->edit_active) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_placement_parse(placement_id, &placement) != UMI_STATUS_OK ||
        placement == UMI_UI_PLACEMENT_FLOATING ||
        !umi_ui_workspace_region_supported(placement)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    layout = umi_ui_workspace_customisation_active(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    window = umi_ui_workspace_layout_find_window_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* A dock operation updates geometry, region and tab membership as one
     * logical transaction so renderers never observe a half-docked panel. */
    before = *layout;
    bounds = umi_ui_workspace_region_rect(placement);
    status = umi_ui_workspace_layout_set_floating(
        layout, window_id, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_placement(
            layout, window_id, placement_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_stack(
            layout, window_id, stack_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_place_window(
            layout,
            window_id,
            bounds.x,
            bounds.y,
            bounds.width,
            bounds.height);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_visible(
            layout, window_id, true);
    }
    return finish_layout_mutation(customisation, layout, &before, status);
}

/*
 * Provide the ui workspace customisation float window operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workspace_customisation_float_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    double x,
    double y,
    double width,
    double height)
{
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceLayout before;
    UmiUiWorkspaceWindow *window;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || window_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!customisation->edit_active) {
        return UMI_STATUS_INVALID_STATE;
    }
    layout = umi_ui_workspace_customisation_active(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    window = umi_ui_workspace_layout_find_window_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Validate and publish floating geometry through the existing layout
     * contract.  A rollback copy protects the prior docked arrangement. */
    before = *layout;
    status = umi_ui_workspace_layout_place_window(
        layout, window_id, x, y, width, height);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_floating(
            layout, window_id, true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_placement(
            layout, window_id, "floating");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        /* A detached panel starts its own tab stack and may later accept
         * another panel through a normal docking operation. */
        status = umi_ui_workspace_layout_set_stack(
            layout, window_id, window_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_visible(
            layout, window_id, true);
    }
    return finish_layout_mutation(customisation, layout, &before, status);
}

/*
 * Provide the ui workspace customisation set auto hidden operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workspace_customisation_set_auto_hidden(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    bool auto_hidden)
{
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceLayout before;
    UmiUiWorkspaceWindow *window;
    const size_t prefix_length = strlen(UMI_UI_AUTO_HIDE_PREFIX);
    char placement[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || window_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!customisation->edit_active) {
        return UMI_STATUS_INVALID_STATE;
    }
    layout = umi_ui_workspace_customisation_active(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    window = umi_ui_workspace_layout_find_window_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (auto_hidden && window->floating) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (auto_hidden ==
        (strncmp(window->placement_id,
                 UMI_UI_AUTO_HIDE_PREFIX,
                 prefix_length) == 0)) {
        return UMI_STATUS_OK;
    }

    /* Auto-hide is encoded as a reversible placement prefix, keeping the
     * stable workspace record unchanged while preserving its dock region. */
    if (auto_hidden) {
        int written = snprintf(
            placement,
            sizeof(placement),
            "%s%s",
            UMI_UI_AUTO_HIDE_PREFIX,
            window->placement_id[0] != '\0'
                ? window->placement_id
                : "centre");
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(placement)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = customisation_copy_text(
            placement,
            sizeof(placement),
            window->placement_id + prefix_length);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    before = *layout;
    status = umi_ui_workspace_layout_set_placement(
        layout, window_id, placement);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_visible(
            layout, window_id, !auto_hidden);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_pinned(
            layout, window_id, !auto_hidden);
    }
    return finish_layout_mutation(customisation, layout, &before, status);
}

/*
 * Provide the ui workspace customisation window is auto hidden operation used by this
 * module and its client applications.
 */
bool umi_ui_workspace_customisation_window_is_auto_hidden(
    const UmiUiWorkspaceCustomisation *customisation,
    const char *window_id)
{
    const UmiUiWorkspaceLayout *layout;
    const UmiUiWorkspaceWindow *window;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || window_id == NULL) {
        return false;
    }
    layout = umi_ui_workspace_customisation_active_const(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) {
        return false;
    }
    window = umi_ui_workspace_layout_find_window(layout, window_id);
    return window != NULL &&
           strncmp(window->placement_id,
                   UMI_UI_AUTO_HIDE_PREFIX,
           strlen(UMI_UI_AUTO_HIDE_PREFIX)) == 0;
}

/* Build a complete, valid starting request for one centre-docked panel. */
UmiUiWorkspacePanelSettings umi_ui_workspace_panel_settings_default(
    const char *window_id)
{
    UmiUiWorkspacePanelSettings settings;
    UmiUiWorkspaceRect rectangle =
        umi_ui_workspace_region_rect(UMI_UI_PLACEMENT_CENTRE);

    /* Zero-initialisation gives optional text fields an unambiguous empty
     * value and prevents future structure growth from exposing stale bytes. */
    (void)memset(&settings, 0, sizeof(settings));
    settings.window_id = window_id;
    settings.placement_id = "centre";
    settings.stack_id = "centre";
    settings.context_group_id = "";
    settings.context_role = UMI_UI_WINDOW_GROUP_BIDIRECTIONAL;
    settings.x = rectangle.x;
    settings.y = rectangle.y;
    settings.width = rectangle.width;
    settings.height = rectangle.height;
    return settings;
}

/* Apply a complete panel request to a candidate and publish it atomically. */
UmiStatus umi_ui_workspace_customisation_apply_panel_settings(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspacePanelSettings *settings)
{
    UmiUiWorkspaceCustomisation *candidate;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || settings == NULL ||
        settings->window_id == NULL || settings->placement_id == NULL ||
        settings->stack_id == NULL || settings->context_group_id == NULL ||
        settings->window_id[0] == '\0' || settings->placement_id[0] == '\0' ||
        settings->stack_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!customisation->edit_active) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Work on a heap-backed candidate because a customisation object contains
     * many complete layouts and is intentionally too large for a safe stack
     * copy on common desktop toolchains. The live model is published only
     * after every requested change succeeds. */
    candidate = (UmiUiWorkspaceCustomisation *)malloc(sizeof(*candidate));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (candidate == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *candidate = *customisation;

    /* Floating panels use the caller's bounded geometry; docked panels use the
     * semantic region and stack so every frontend can choose its own pixels. */
    if (settings->floating) {
        status = umi_ui_workspace_customisation_float_window(
            candidate,
            settings->window_id,
            settings->x,
            settings->y,
            settings->width,
            settings->height);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_ui_workspace_customisation_dock_window(
            candidate,
            settings->window_id,
            settings->placement_id,
            settings->stack_id);
    }

    /* Auto-hide is applied after placement because a floating panel cannot be
     * collapsed into a dock strip. The lower-level guard rejects that conflict. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_customisation_set_auto_hidden(
            candidate, settings->window_id, settings->auto_hidden);
    }

    /* An empty context ID means the user explicitly selected no linked group;
     * otherwise the requested source/destination role is recorded atomically. */
    if (status == UMI_STATUS_OK && settings->context_group_id[0] == '\0') {
        status = umi_ui_workspace_customisation_clear_context_group(
            candidate, settings->window_id);
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_customisation_assign_context_group(
            candidate,
            settings->window_id,
            settings->context_group_id,
            settings->context_role);
    }

    /* Publish only a fully valid candidate. Any error leaves the original
     * model, its edit baseline and its linked-context membership untouched. */
    if (status == UMI_STATUS_OK) {
        *customisation = *candidate;
    }
    free(candidate);
    return status;
}

/*
 * Provide the ui workspace customisation close window operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workspace_customisation_close_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || window_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!customisation->edit_active) {
        return UMI_STATUS_INVALID_STATE;
    }
    layout = umi_ui_workspace_customisation_active(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Removing an instance does not remove its catalogue descriptor, so the
     * user can reopen it later from New Window. */
    status = umi_ui_workspace_layout_remove_window(layout, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        customisation->revision += 1U;
    }
    return status;
}
/*
 * Provide the ui workspace customisation set theme operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_set_theme(UmiUiWorkspaceCustomisation *customisation,const UmiUiThemeProfile *theme)
{ char reason[192U]; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (customisation == NULL || theme == NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (umi_ui_theme_profile_validate(theme,reason,sizeof(reason)) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE; customisation->theme = *theme; customisation->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the ui workspace customisation assign context group operation used by this
 * module and its client applications.
 */
UmiStatus umi_ui_workspace_customisation_assign_context_group(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    const char *context_group_id,
    UmiUiWindowGroupRole role)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || window_id == NULL ||
        context_group_id == NULL || context_group_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!customisation->edit_active) return UMI_STATUS_INVALID_STATE;
    layout = umi_ui_workspace_customisation_active(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL ||
        umi_ui_workspace_layout_find_window(layout, window_id) == NULL ||
        umi_ui_window_group_find(&customisation->groups, context_group_id) ==
            NULL)
        return UMI_STATUS_NOT_FOUND;
    status = umi_ui_window_group_assign(
        &customisation->groups, context_group_id, window_id, role);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_context_group(
            layout, window_id, context_group_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
        (void)umi_ui_window_group_unassign(&customisation->groups, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) customisation->revision += 1U;
    return status;
}

/*
 * Provide the ui workspace customisation clear context group operation used by this module
 * and its client applications.
 */
UmiStatus umi_ui_workspace_customisation_clear_context_group(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!customisation->edit_active) return UMI_STATUS_INVALID_STATE;
    layout = umi_ui_workspace_customisation_active(customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL ||
        umi_ui_workspace_layout_find_window(layout, window_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    status = umi_ui_window_group_unassign(&customisation->groups, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ui_workspace_layout_set_context_group(
            layout, window_id, "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) customisation->revision += 1U;
    return status;
}
/*
 * Provide the ui workspace customisation snapshot operation used by this module and its
 * client applications.
 */
void umi_ui_workspace_customisation_snapshot(const UmiUiWorkspaceCustomisation *customisation,UmiUiWorkspaceCustomisationSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_snapshot == NULL) return;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (customisation == NULL) return;
    out_snapshot->layouts = customisation->layout_count; out_snapshot->available_windows = customisation->windows.count; out_snapshot->recent_windows = customisation->windows.recent_count; out_snapshot->groups = customisation->groups.count; out_snapshot->presets = customisation->library.count; (void)snprintf(out_snapshot->active_layout_id,sizeof(out_snapshot->active_layout_id),"%s",customisation->active_layout_id); (void)snprintf(out_snapshot->theme_id,sizeof(out_snapshot->theme_id),"%s",customisation->theme.theme_id); out_snapshot->editing = customisation->edit_active; out_snapshot->revision = customisation->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < customisation->layout_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(customisation->layouts[index].layout_id,customisation->active_layout_id) == 0) out_snapshot->active_layout_locked = customisation->layouts[index].locked;
}

/* Reject malformed bounded records before existing lookup operations can walk
 * them. Canvas commands are synchronous and require exclusive controller
 * ownership, as do the established customisation operations. */
static UmiStatus canvas_records_validate(
    const UmiUiWorkspaceCustomisation *customisation)
{
    size_t index;
    size_t member;

    if (customisation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (customisation->layout_count > UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS ||
        customisation->groups.count > UMI_UI_WINDOW_GROUP_MAX ||
        memchr(customisation->active_layout_id, '\0',
               sizeof(customisation->active_layout_id)) == NULL)
        return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < customisation->layout_count; ++index) {
        const UmiUiWorkspaceLayout *layout = &customisation->layouts[index];
        char reason[192U];
        if (layout->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS ||
            layout->layout_id[0] == '\0' ||
            memchr(layout->layout_id, '\0', sizeof(layout->layout_id)) == NULL)
            return UMI_STATUS_INVALID_STATE;
        for (member = 0U; member < index; ++member) {
            if (strcmp(layout->layout_id,
                       customisation->layouts[member].layout_id) == 0)
                return UMI_STATUS_INVALID_STATE;
        }
        for (member = 0U; member < layout->window_count; ++member) {
            const UmiUiWorkspaceWindow *window = &layout->windows[member];
            size_t earlier;
            if (window->window_id[0] == '\0' ||
                memchr(window->window_id, '\0', sizeof(window->window_id)) == NULL)
                return UMI_STATUS_INVALID_STATE;
            for (earlier = 0U; earlier < member; ++earlier) {
                if (strcmp(window->window_id,
                           layout->windows[earlier].window_id) == 0)
                    return UMI_STATUS_INVALID_STATE;
            }
        }
        if (umi_ui_workspace_layout_validate(layout, reason, sizeof(reason)) !=
            UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U; index < customisation->groups.count; ++index) {
        const UmiUiWindowGroup *group = &customisation->groups.items[index];
        if (group->member_count > UMI_UI_WINDOW_GROUP_MAX_MEMBERS)
            return UMI_STATUS_INVALID_STATE;
        for (member = 0U; member < group->member_count; ++member) {
            size_t earlier;
            if (group->members[member].window_id[0] == '\0' ||
                memchr(group->members[member].window_id, '\0',
                       sizeof(group->members[member].window_id)) == NULL)
                return UMI_STATUS_INVALID_STATE;
            for (earlier = 0U; earlier < member; ++earlier) {
                if (strcmp(group->members[member].window_id,
                           group->members[earlier].window_id) == 0)
                    return UMI_STATUS_INVALID_STATE;
            }
        }
    }
    return UMI_STATUS_OK;
}

/* Keep another saved layout's context links when legacy layouts reuse the
 * same instance ID. Only an ID absent from every surviving layout is orphaned. */
static bool canvas_window_still_referenced(
    const UmiUiWorkspaceCustomisation *customisation,
    const char *window_id)
{
    size_t index;
    for (index = 0U; index < customisation->layout_count; ++index) {
        if (umi_ui_workspace_layout_find_window(
                &customisation->layouts[index], window_id) != NULL)
            return true;
    }
    return false;
}

/* Build an empty candidate through the existing layout API. Publish the
 * complete result once, so activation failure never leaves a hidden new tab. */
UmiStatus umi_ui_workspace_customisation_create_blank_layout(
    UmiUiWorkspaceCustomisation *customisation,
    const char *layout_id,
    const char *name)
{
    UmiUiWorkspaceCustomisation *candidate;
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    size_t index;

    if (customisation == NULL || layout_id == NULL || name == NULL ||
        layout_id[0] == '\0' || name[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = canvas_records_validate(customisation);
    if (status != UMI_STATUS_OK) return status;
    if (customisation->edit_active) return UMI_STATUS_BUSY;
    if (customisation->layout_count >= UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS ||
        customisation->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < customisation->layout_count; ++index) {
        if (strcmp(customisation->layouts[index].layout_id, layout_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    }
    candidate = (UmiUiWorkspaceCustomisation *)malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *customisation;
    layout = &candidate->layouts[candidate->layout_count];
    status = umi_ui_workspace_layout_init(layout, layout_id, name);
    if (status == UMI_STATUS_OK) {
        status = customisation_copy_text(
            candidate->active_layout_id, sizeof(candidate->active_layout_id),
            layout->layout_id);
    }
    if (status == UMI_STATUS_OK) {
        candidate->layout_count += 1U;
        candidate->revision += 1U;
        *customisation = *candidate;
    }
    free(candidate);
    return status;
}

/* Clear the candidate, not the live layout. The caller's outer edit baseline
 * is deliberately preserved, including the reverse context membership. */
UmiStatus umi_ui_workspace_customisation_clear_canvas(
    UmiUiWorkspaceCustomisation *customisation,
    UmiUiWorkspaceCanvasClearResult *out_result)
{
    UmiUiWorkspaceCustomisation *candidate;
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceCanvasClearResult result = {0U, 0U};
    UmiStatus status = canvas_records_validate(customisation);
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    if (!customisation->edit_active) return UMI_STATUS_INVALID_STATE;
    layout = umi_ui_workspace_customisation_active(customisation);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        if (window->closable && !window->pinned) result.removed += 1U;
        else result.retained += 1U;
    }
    if (result.removed == 0U) {
        if (out_result != NULL) *out_result = result;
        return UMI_STATUS_OK;
    }
    if (customisation->revision == UINT64_MAX ||
        layout->revision > UINT64_MAX - (uint64_t)result.removed ||
        customisation->groups.revision > UINT64_MAX - (uint64_t)result.removed)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < customisation->groups.count; ++index) {
        if (customisation->groups.items[index].revision >
            UINT64_MAX - (uint64_t)result.removed)
            return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    candidate = (UmiUiWorkspaceCustomisation *)malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *customisation;
    layout = umi_ui_workspace_customisation_active(candidate);
    /* Reverse traversal preserves relative order and avoids shifted indexes. */
    for (index = layout->window_count; index > 0U; --index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index - 1U];
        char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
        if (!window->closable || window->pinned) continue;
        /* Copy before remove: window_id must survive the array compaction. */
        (void)memcpy(window_id, window->window_id, sizeof(window_id));
        status = umi_ui_workspace_layout_remove_window(layout, window_id);
        if (status != UMI_STATUS_OK) break;
        if (!canvas_window_still_referenced(candidate, window_id)) {
            status = umi_ui_window_group_unassign(&candidate->groups, window_id);
            if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_OK;
            if (status != UMI_STATUS_OK) break;
        }
    }
    if (status == UMI_STATUS_OK) {
        /* Erase inactive slots so removed instance metadata is not retained in
         * a later whole-record checkpoint. Product storage is not accessed. */
        (void)memset(&layout->windows[layout->window_count], 0,
            (UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS - layout->window_count) *
            sizeof(layout->windows[0]));
        candidate->revision += 1U;
        *customisation = *candidate;
        if (out_result != NULL) *out_result = result;
    }
    free(candidate);
    return status;
}

/* Record free placement without changing the existing detached-window flag.
 * The complete window value is prepared locally and published in one step. */
UmiStatus umi_ui_workspace_customisation_place_canvas_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    double x,
    double y,
    double width,
    double height)
{
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceWindow *window;
    UmiUiWorkspaceWindow candidate;
    UmiStatus status;

    if (customisation == NULL || window_id == NULL || window_id[0] == '\0' ||
        !isfinite(x) || !isfinite(y) || !isfinite(width) || !isfinite(height) ||
        x < 0.0 || y < 0.0 || width <= 0.0 || height <= 0.0 ||
        width > 1.0 || height > 1.0 || x > 1.0 - width || y > 1.0 - height)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = canvas_records_validate(customisation);
    if (status != UMI_STATUS_OK) return status;
    if (!customisation->edit_active) return UMI_STATUS_INVALID_STATE;
    layout = umi_ui_workspace_customisation_active(customisation);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = umi_ui_workspace_layout_find_window_mutable(layout, window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    if (window->pinned || (!window->resizable &&
        (window->width != width || window->height != height)))
        return UMI_STATUS_PERMISSION_DENIED;
    if (customisation->revision == UINT64_MAX || layout->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate = *window;
    status = customisation_copy_text(candidate.placement_id,
        sizeof(candidate.placement_id), UMI_UI_WORKSPACE_CANVAS_PLACEMENT);
    if (status == UMI_STATUS_OK)
        status = customisation_copy_text(candidate.stack_id,
            sizeof(candidate.stack_id), candidate.window_id);
    if (status == UMI_STATUS_OK)
        status = customisation_copy_text(candidate.group_id,
            sizeof(candidate.group_id), candidate.window_id);
    if (status != UMI_STATUS_OK) return status;
    candidate.x = x;
    candidate.y = y;
    candidate.width = width;
    candidate.height = height;
    candidate.floating = false;
    candidate.visible = true;
    candidate.maximised = false;
    *window = candidate;
    layout->revision += 1U;
    customisation->revision += 1U;
    return UMI_STATUS_OK;
}

