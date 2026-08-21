/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/node.c
 *
 * PURPOSE:
 *   Implement semantic node construction, validation, flags, sizing and child ordering without storing toolkit widget pointers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/node.h"

#include <string.h>

#include "internal.h"

static bool node_kind_valid(UmiWorkbenchLayoutNodeKind kind)
{
    return kind >= UMI_WORKBENCH_LAYOUT_NODE_EMPTY &&
           kind <= UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW;
}

static bool orientation_valid(UmiWorkbenchLayoutOrientation orientation)
{
    return orientation >= UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE &&
           orientation <= UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL;
}

static bool dock_region_valid(UmiWorkbenchLayoutDockRegion region)
{
    return region >= UMI_WORKBENCH_LAYOUT_DOCK_CANVAS &&
           region <= UMI_WORKBENCH_LAYOUT_DOCK_FLOATING;
}

static bool visibility_valid(UmiWorkbenchLayoutVisibility visibility)
{
    return visibility >= UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE &&
           visibility <= UMI_WORKBENCH_LAYOUT_VISIBILITY_AUTO;
}

void umi_workbench_layout_node_init(
    UmiWorkbenchLayoutNode *node,
    const char *node_id,
    UmiWorkbenchLayoutNodeKind kind)
{
    size_t index;

    if (node == NULL) {
        return;
    }

    (void)memset(node, 0, sizeof(*node));
    node->structure_size = sizeof(*node);
    node->kind = kind;
    node->orientation = UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE;
    node->dock_region = UMI_WORKBENCH_LAYOUT_DOCK_CANVAS;
    node->visibility = UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE;
    node->split_ratio = 0.5;
    node->parent_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    node->active_child_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    node->minimum_size.width = 0;
    node->minimum_size.height = 0;
    node->preferred_size.width = 0;
    node->preferred_size.height = 0;
    node->flags = UMI_WORKBENCH_LAYOUT_NODE_CLOSABLE |
                  UMI_WORKBENCH_LAYOUT_NODE_MOVABLE |
                  UMI_WORKBENCH_LAYOUT_NODE_RESIZABLE;
    node->revision = 1U;

    for (index = 0U; index < UMI_WORKBENCH_LAYOUT_MAX_CHILDREN; ++index) {
        node->child_indices[index] = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }

    if (node_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            node->node_id, sizeof(node->node_id), node_id, true);
    }
}

UmiStatus umi_workbench_layout_node_copy(
    UmiWorkbenchLayoutNode *destination,
    const UmiWorkbenchLayoutNode *source)
{
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (source->structure_size < sizeof(*source)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *destination = *source;
    destination->structure_size = sizeof(*destination);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_node_validate(
    const UmiWorkbenchLayoutNode *node)
{
    size_t index;

    if (node == NULL || node->structure_size < sizeof(*node)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_layout_text_present(node->node_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!node_kind_valid(node->kind) ||
        !orientation_valid(node->orientation) ||
        !dock_region_valid(node->dock_region) ||
        !visibility_valid(node->visibility)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (node->child_count > UMI_WORKBENCH_LAYOUT_MAX_CHILDREN) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (node->minimum_size.width < 0 || node->minimum_size.height < 0 ||
        node->preferred_size.width < 0 ||
        node->preferred_size.height < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_layout_rect_is_valid(&node->bounds)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT) {
        if (node->orientation == UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE ||
            node->split_ratio < 0.05 || node->split_ratio > 0.95) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    if (umi_workbench_layout_node_is_leaf(node) &&
        node->child_count != 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (node->active_child_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
        node->active_child_index >= node->child_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < node->child_count; ++index) {
        size_t other;
        if (node->child_indices[index] == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        for (other = index + 1U; other < node->child_count; ++other) {
            if (node->child_indices[index] == node->child_indices[other]) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    if ((node->kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
         node->kind == UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP) &&
        (!umi_workbench_layout_text_present(node->component_id) ||
         !umi_workbench_layout_text_present(node->owner_application_id))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

bool umi_workbench_layout_node_is_container(
    const UmiWorkbenchLayoutNode *node)
{
    if (node == NULL) {
        return false;
    }
    return node->kind == UMI_WORKBENCH_LAYOUT_NODE_WINDOW ||
           node->kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT ||
           node->kind == UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP ||
           node->kind == UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW;
}

bool umi_workbench_layout_node_is_leaf(
    const UmiWorkbenchLayoutNode *node)
{
    if (node == NULL) {
        return false;
    }
    return node->kind == UMI_WORKBENCH_LAYOUT_NODE_EMPTY ||
           node->kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
           node->kind == UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP;
}

bool umi_workbench_layout_node_accepts_child_kind(
    const UmiWorkbenchLayoutNode *parent,
    UmiWorkbenchLayoutNodeKind child_kind)
{
    if (parent == NULL || !node_kind_valid(child_kind)) {
        return false;
    }

    switch (parent->kind) {
    case UMI_WORKBENCH_LAYOUT_NODE_WINDOW:
        return child_kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_EMPTY;
    case UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW:
        return child_kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_EMPTY;
    case UMI_WORKBENCH_LAYOUT_NODE_SPLIT:
        return child_kind != UMI_WORKBENCH_LAYOUT_NODE_WINDOW &&
               child_kind != UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW;
    case UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP:
        return child_kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP ||
               child_kind == UMI_WORKBENCH_LAYOUT_NODE_EMPTY;
    default:
        return false;
    }
}

bool umi_workbench_layout_node_has_flag(
    const UmiWorkbenchLayoutNode *node,
    UmiWorkbenchLayoutNodeFlags flag)
{
    return node != NULL &&
           (node->flags & (uint32_t)flag) == (uint32_t)flag;
}

UmiStatus umi_workbench_layout_node_set_flag(
    UmiWorkbenchLayoutNode *node,
    UmiWorkbenchLayoutNodeFlags flag,
    bool enabled)
{
    if (node == NULL || flag == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (enabled) {
        node->flags |= (uint32_t)flag;
    } else {
        node->flags &= ~(uint32_t)flag;
    }
    node->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_node_set_title(
    UmiWorkbenchLayoutNode *node,
    const char *title)
{
    UmiStatus status;
    if (node == NULL || title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_copy_text(
        node->title, sizeof(node->title), title, true);
    if (status == UMI_STATUS_OK) {
        node->revision += 1U;
    }
    return status;
}

UmiStatus umi_workbench_layout_node_set_component(
    UmiWorkbenchLayoutNode *node,
    const char *component_id,
    const char *owner_application_id)
{
    UmiStatus status;
    if (node == NULL || component_id == NULL ||
        owner_application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_layout_copy_text(
        node->component_id,
        sizeof(node->component_id),
        component_id,
        true);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            node->owner_application_id,
            sizeof(node->owner_application_id),
            owner_application_id,
            true);
    }
    if (status == UMI_STATUS_OK) {
        node->revision += 1U;
    }
    return status;
}

UmiStatus umi_workbench_layout_node_set_context_group(
    UmiWorkbenchLayoutNode *node,
    const char *context_group_id)
{
    UmiStatus status;
    if (node == NULL || context_group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_copy_text(
        node->context_group_id,
        sizeof(node->context_group_id),
        context_group_id,
        true);
    if (status == UMI_STATUS_OK) {
        node->revision += 1U;
    }
    return status;
}

UmiStatus umi_workbench_layout_node_set_monitor(
    UmiWorkbenchLayoutNode *node,
    const char *monitor_id)
{
    UmiStatus status;
    if (node == NULL || monitor_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_copy_text(
        node->monitor_id,
        sizeof(node->monitor_id),
        monitor_id,
        true);
    if (status == UMI_STATUS_OK) {
        node->revision += 1U;
    }
    return status;
}

UmiStatus umi_workbench_layout_node_set_bounds(
    UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchLayoutRect *bounds)
{
    if (node == NULL || !umi_workbench_layout_rect_is_valid(bounds)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node->bounds = *bounds;
    node->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_node_set_split(
    UmiWorkbenchLayoutNode *node,
    UmiWorkbenchLayoutOrientation orientation,
    double split_ratio)
{
    if (node == NULL ||
        node->kind != UMI_WORKBENCH_LAYOUT_NODE_SPLIT ||
        orientation == UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE ||
        split_ratio < 0.05 || split_ratio > 0.95) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node->orientation = orientation;
    node->split_ratio = split_ratio;
    node->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_node_add_child_index(
    UmiWorkbenchLayoutNode *node,
    size_t child_index,
    size_t position)
{
    size_t index;

    if (node == NULL || !umi_workbench_layout_node_is_container(node) ||
        child_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (node->child_count >= UMI_WORKBENCH_LAYOUT_MAX_CHILDREN) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_workbench_layout_node_contains_child_index(node, child_index)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (position == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        position = node->child_count;
    }
    if (position > node->child_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = node->child_count; index > position; --index) {
        node->child_indices[index] = node->child_indices[index - 1U];
    }
    node->child_indices[position] = child_index;
    node->child_count += 1U;
    if (node->active_child_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        node->active_child_index = 0U;
    } else if (position <= node->active_child_index) {
        node->active_child_index += 1U;
    }
    node->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_node_remove_child_index(
    UmiWorkbenchLayoutNode *node,
    size_t child_index)
{
    size_t position;
    size_t index;

    if (node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = umi_workbench_layout_node_child_position(node, child_index);
    if (position == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }

    for (index = position; index + 1U < node->child_count; ++index) {
        node->child_indices[index] = node->child_indices[index + 1U];
    }
    node->child_count -= 1U;
    node->child_indices[node->child_count] =
        UMI_WORKBENCH_LAYOUT_INDEX_NONE;

    if (node->child_count == 0U) {
        node->active_child_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    } else if (node->active_child_index == position) {
        node->active_child_index =
            position < node->child_count ? position : node->child_count - 1U;
    } else if (node->active_child_index > position) {
        node->active_child_index -= 1U;
    }

    node->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_node_replace_child_index(
    UmiWorkbenchLayoutNode *node,
    size_t existing_child_index,
    size_t replacement_child_index)
{
    size_t position;
    if (node == NULL ||
        replacement_child_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_layout_node_contains_child_index(
            node, replacement_child_index)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    position = umi_workbench_layout_node_child_position(
        node, existing_child_index);
    if (position == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    node->child_indices[position] = replacement_child_index;
    node->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_node_move_child_index(
    UmiWorkbenchLayoutNode *node,
    size_t child_index,
    size_t new_position)
{
    size_t old_position;
    size_t index;

    if (node == NULL || new_position >= node->child_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    old_position = umi_workbench_layout_node_child_position(
        node, child_index);
    if (old_position == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (old_position == new_position) {
        return UMI_STATUS_OK;
    }

    if (old_position < new_position) {
        for (index = old_position; index < new_position; ++index) {
            node->child_indices[index] = node->child_indices[index + 1U];
        }
    } else {
        for (index = old_position; index > new_position; --index) {
            node->child_indices[index] = node->child_indices[index - 1U];
        }
    }
    node->child_indices[new_position] = child_index;

    if (node->active_child_index == old_position) {
        node->active_child_index = new_position;
    } else if (old_position < node->active_child_index &&
               new_position >= node->active_child_index) {
        node->active_child_index -= 1U;
    } else if (old_position > node->active_child_index &&
               new_position <= node->active_child_index) {
        node->active_child_index += 1U;
    }

    node->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_workbench_layout_node_contains_child_index(
    const UmiWorkbenchLayoutNode *node,
    size_t child_index)
{
    return umi_workbench_layout_node_child_position(node, child_index) !=
           UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

size_t umi_workbench_layout_node_child_position(
    const UmiWorkbenchLayoutNode *node,
    size_t child_index)
{
    size_t index;

    if (node == NULL) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    for (index = 0U; index < node->child_count; ++index) {
        if (node->child_indices[index] == child_index) {
            return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}
