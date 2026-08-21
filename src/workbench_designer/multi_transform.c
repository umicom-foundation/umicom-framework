/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/multi_transform.c
 *
 * PURPOSE:
 *   Capture, calculate and apply multi-node transforms while preserving integer
 *   semantic geometry and document revision evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/multi_transform.h"
#include "internal.h"
#include <math.h>


void umi_workbench_designer_multi_transform_init(
    UmiWorkbenchDesignerMultiTransform *transform)
{
    if (transform == NULL) return;
    (void)memset(transform, 0, sizeof(*transform));
}

static UmiWorkbenchDesignerRect transform_union(
    UmiWorkbenchDesignerRect left,
    UmiWorkbenchDesignerRect right,
    bool first)
{
    UmiWorkbenchDesignerRect result;
    double right_edge;
    double bottom_edge;
    if (first) return right;
    result.x = left.x < right.x ? left.x : right.x;
    result.y = left.y < right.y ? left.y : right.y;
    right_edge = (left.x + left.width) > (right.x + right.width)
        ? left.x + left.width : right.x + right.width;
    bottom_edge = (left.y + left.height) > (right.y + right.height)
        ? left.y + left.height : right.y + right.height;
    result.width = right_edge - result.x;
    result.height = bottom_edge - result.y;
    return result;
}

UmiStatus umi_workbench_designer_multi_transform_begin(
    UmiWorkbenchDesignerMultiTransform *transform,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection)
{
    size_t index;
    if (transform == NULL || document == NULL || selection == NULL ||
        selection->count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_multi_transform_init(transform);
    for (index = 0U; index < selection->count; ++index) {
        const char *node_id = selection->items[index].value;
        const UmiWorkbenchLayoutNode *node =
            umi_workbench_layout_document_find_node(document, node_id);
        UmiWorkbenchDesignerTransformItem *item;
        UmiWorkbenchDesignerRect bounds;
        if (node == NULL) return UMI_STATUS_NOT_FOUND;
        if (transform->count >= UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        item = &transform->items[transform->count];
        (void)memset(item, 0, sizeof(*item));
        (void)umi_workbench_designer_copy_text(
            item->node_id, sizeof(item->node_id), node_id);
        item->original_bounds = node->bounds;
        item->resulting_bounds = node->bounds;
        bounds = umi_workbench_designer_from_layout_rect(node->bounds);
        transform->original_selection_bounds = transform_union(
            transform->original_selection_bounds, bounds,
            transform->count == 0U);
        transform->count += 1U;
    }
    transform->resulting_selection_bounds = transform->original_selection_bounds;
    transform->source_revision = document->version.revision;
    transform->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_multi_transform_translate(
    UmiWorkbenchDesignerMultiTransform *transform,
    double delta_x,
    double delta_y)
{
    size_t index;
    if (transform == NULL || transform->count == 0U ||
        !isfinite(delta_x) || !isfinite(delta_y)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < transform->count; ++index) {
        UmiWorkbenchDesignerTransformItem *item = &transform->items[index];
        item->resulting_bounds = item->original_bounds;
        item->resulting_bounds.x += (int32_t)llround(delta_x);
        item->resulting_bounds.y += (int32_t)llround(delta_y);
        item->changed = delta_x != 0.0 || delta_y != 0.0;
    }
    transform->resulting_selection_bounds = transform->original_selection_bounds;
    transform->resulting_selection_bounds.x += delta_x;
    transform->resulting_selection_bounds.y += delta_y;
    transform->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_multi_transform_resize(
    UmiWorkbenchDesignerMultiTransform *transform,
    UmiWorkbenchDesignerRect new_bounds,
    bool preserve_aspect)
{
    double scale_x;
    double scale_y;
    size_t index;
    if (transform == NULL || transform->count == 0U ||
        !umi_workbench_designer_rect_is_valid(&new_bounds) ||
        transform->original_selection_bounds.width <= 0.0 ||
        transform->original_selection_bounds.height <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    scale_x = new_bounds.width / transform->original_selection_bounds.width;
    scale_y = new_bounds.height / transform->original_selection_bounds.height;
    if (preserve_aspect) {
        const double scale = scale_x < scale_y ? scale_x : scale_y;
        scale_x = scale;
        scale_y = scale;
        new_bounds.width = transform->original_selection_bounds.width * scale;
        new_bounds.height = transform->original_selection_bounds.height * scale;
    }
    for (index = 0U; index < transform->count; ++index) {
        UmiWorkbenchDesignerTransformItem *item = &transform->items[index];
        const UmiWorkbenchDesignerRect original =
            umi_workbench_designer_from_layout_rect(item->original_bounds);
        UmiWorkbenchDesignerRect result;
        result.x = new_bounds.x +
            (original.x - transform->original_selection_bounds.x) * scale_x;
        result.y = new_bounds.y +
            (original.y - transform->original_selection_bounds.y) * scale_y;
        result.width = original.width * scale_x;
        result.height = original.height * scale_y;
        item->resulting_bounds = umi_workbench_designer_to_layout_rect(result);
        item->changed = memcmp(
            &item->resulting_bounds, &item->original_bounds,
            sizeof(item->original_bounds)) != 0;
    }
    transform->resulting_selection_bounds = new_bounds;
    transform->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_multi_transform_apply(
    const UmiWorkbenchDesignerMultiTransform *transform,
    UmiWorkbenchLayoutDocument *document)
{
    size_t index;
    if (transform == NULL || document == NULL || transform->count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (document->version.revision != transform->source_revision) {
        return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U; index < transform->count; ++index) {
        const UmiWorkbenchDesignerTransformItem *item = &transform->items[index];
        UmiWorkbenchLayoutNode *node =
            umi_workbench_layout_document_find_node_mutable(document, item->node_id);
        UmiStatus status;
        if (node == NULL) return UMI_STATUS_NOT_FOUND;
        if (!item->changed) continue;
        status = umi_workbench_layout_node_set_bounds(
            node, &item->resulting_bounds);
        if (status != UMI_STATUS_OK) return status;
    }
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}
