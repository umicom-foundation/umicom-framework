/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/alignment.c
 *
 * PURPOSE:
 *   Apply alignment and distribution to explicit layout bounds while preserving
 *   stable node identities and revision evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/alignment.h"
#include "internal.h"
#include <math.h>


static bool alignment_selected(
    const UmiWorkbenchDesignerSelection *selection,
    const UmiWorkbenchLayoutNode *node)
{
    return selection != NULL && node != NULL &&
        umi_workbench_designer_selection_contains(selection, node->node_id);
}

static UmiWorkbenchDesignerRect alignment_bounds(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection)
{
    UmiWorkbenchDesignerRect result = {0.0, 0.0, 0.0, 0.0};
    bool first = true;
    size_t index;
    for (index = 0U; index < document->node_count; ++index) {
        const UmiWorkbenchLayoutNode *node = &document->nodes[index];
        UmiWorkbenchDesignerRect rect;
        double right;
        double bottom;
        if (!alignment_selected(selection, node)) continue;
        rect = umi_workbench_designer_from_layout_rect(node->bounds);
        if (!umi_workbench_designer_rect_is_valid(&rect)) continue;
        if (first) {
            result = rect;
            first = false;
            continue;
        }
        right = fmax(result.x + result.width, rect.x + rect.width);
        bottom = fmax(result.y + result.height, rect.y + rect.height);
        result.x = fmin(result.x, rect.x);
        result.y = fmin(result.y, rect.y);
        result.width = right - result.x;
        result.height = bottom - result.y;
    }
    return result;
}

static double alignment_target(
    UmiWorkbenchDesignerAlignment alignment,
    UmiWorkbenchDesignerRect bounds)
{
    switch (alignment) {
        case UMI_WORKBENCH_DESIGNER_ALIGN_LEFT: return bounds.x;
        case UMI_WORKBENCH_DESIGNER_ALIGN_HORIZONTAL_CENTRE:
            return bounds.x + (bounds.width / 2.0);
        case UMI_WORKBENCH_DESIGNER_ALIGN_RIGHT:
            return bounds.x + bounds.width;
        case UMI_WORKBENCH_DESIGNER_ALIGN_TOP: return bounds.y;
        case UMI_WORKBENCH_DESIGNER_ALIGN_VERTICAL_CENTRE:
            return bounds.y + (bounds.height / 2.0);
        case UMI_WORKBENCH_DESIGNER_ALIGN_BOTTOM:
            return bounds.y + bounds.height;
        default: return 0.0;
    }
}

UmiStatus umi_workbench_designer_align_selection(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    UmiWorkbenchDesignerAlignment alignment,
    UmiWorkbenchDesignerAlignmentResult *out_result)
{
    UmiWorkbenchDesignerRect bounds;
    double target;
    size_t index;
    if (document == NULL || selection == NULL || out_result == NULL ||
        selection->count < 2U || alignment < UMI_WORKBENCH_DESIGNER_ALIGN_LEFT ||
        alignment > UMI_WORKBENCH_DESIGNER_ALIGN_BOTTOM) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_layout_document_has_flag(
            document, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->previous_revision = document->version.revision;
    bounds = alignment_bounds(document, selection);
    if (!umi_workbench_designer_rect_is_valid(&bounds)) {
        return UMI_STATUS_INVALID_STATE;
    }
    target = alignment_target(alignment, bounds);
    for (index = 0U; index < document->node_count; ++index) {
        UmiWorkbenchLayoutNode *node = &document->nodes[index];
        UmiWorkbenchDesignerRect rect;
        UmiWorkbenchLayoutRect layout_rect;
        if (!alignment_selected(selection, node)) continue;
        rect = umi_workbench_designer_from_layout_rect(node->bounds);
        switch (alignment) {
            case UMI_WORKBENCH_DESIGNER_ALIGN_LEFT: rect.x = target; break;
            case UMI_WORKBENCH_DESIGNER_ALIGN_HORIZONTAL_CENTRE:
                rect.x = target - (rect.width / 2.0); break;
            case UMI_WORKBENCH_DESIGNER_ALIGN_RIGHT:
                rect.x = target - rect.width; break;
            case UMI_WORKBENCH_DESIGNER_ALIGN_TOP: rect.y = target; break;
            case UMI_WORKBENCH_DESIGNER_ALIGN_VERTICAL_CENTRE:
                rect.y = target - (rect.height / 2.0); break;
            case UMI_WORKBENCH_DESIGNER_ALIGN_BOTTOM:
                rect.y = target - rect.height; break;
            default: return UMI_STATUS_INVALID_ARGUMENT;
        }
        layout_rect = umi_workbench_designer_to_layout_rect(rect);
        if (umi_workbench_layout_node_set_bounds(node, &layout_rect) ==
            UMI_STATUS_OK) {
            node->revision += 1U;
            out_result->changed_count += 1U;
        }
    }
    if (out_result->changed_count > 0U) {
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
    }
    out_result->resulting_bounds = alignment_bounds(document, selection);
    out_result->resulting_revision = document->version.revision;
    return UMI_STATUS_OK;
}

static void alignment_sort_indices(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    bool horizontal,
    size_t *indices,
    size_t *out_count)
{
    size_t count = 0U;
    size_t index;
    size_t left;
    size_t right;
    for (index = 0U; index < document->node_count; ++index) {
        if (alignment_selected(selection, &document->nodes[index])) {
            indices[count++] = index;
        }
    }
    for (left = 1U; left < count; ++left) {
        const size_t value = indices[left];
        const UmiWorkbenchDesignerRect value_rect =
            umi_workbench_designer_from_layout_rect(document->nodes[value].bounds);
        const double value_key = horizontal ? value_rect.x : value_rect.y;
        right = left;
        while (right > 0U) {
            const UmiWorkbenchDesignerRect previous_rect =
                umi_workbench_designer_from_layout_rect(
                    document->nodes[indices[right - 1U]].bounds);
            const double previous_key = horizontal
                ? previous_rect.x : previous_rect.y;
            if (previous_key <= value_key) break;
            indices[right] = indices[right - 1U];
            --right;
        }
        indices[right] = value;
    }
    *out_count = count;
}

UmiStatus umi_workbench_designer_distribute_selection(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    UmiWorkbenchDesignerDistribution distribution,
    UmiWorkbenchDesignerAlignmentResult *out_result)
{
    size_t indices[UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS];
    size_t count;
    size_t position;
    bool horizontal;
    bool gaps;
    UmiWorkbenchDesignerRect first;
    UmiWorkbenchDesignerRect last;
    double step;
    double occupied = 0.0;
    if (document == NULL || selection == NULL || out_result == NULL ||
        selection->count < 3U ||
        distribution < UMI_WORKBENCH_DESIGNER_DISTRIBUTE_HORIZONTAL_CENTRES ||
        distribution > UMI_WORKBENCH_DESIGNER_DISTRIBUTE_VERTICAL_GAPS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_layout_document_has_flag(
            document, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    horizontal = distribution ==
        UMI_WORKBENCH_DESIGNER_DISTRIBUTE_HORIZONTAL_CENTRES ||
        distribution == UMI_WORKBENCH_DESIGNER_DISTRIBUTE_HORIZONTAL_GAPS;
    gaps = distribution == UMI_WORKBENCH_DESIGNER_DISTRIBUTE_HORIZONTAL_GAPS ||
        distribution == UMI_WORKBENCH_DESIGNER_DISTRIBUTE_VERTICAL_GAPS;
    alignment_sort_indices(document, selection, horizontal, indices, &count);
    if (count < 3U) return UMI_STATUS_INVALID_STATE;
    first = umi_workbench_designer_from_layout_rect(
        document->nodes[indices[0U]].bounds);
    last = umi_workbench_designer_from_layout_rect(
        document->nodes[indices[count - 1U]].bounds);
    if (gaps) {
        for (position = 0U; position < count; ++position) {
            UmiWorkbenchDesignerRect rect =
                umi_workbench_designer_from_layout_rect(
                    document->nodes[indices[position]].bounds);
            occupied += horizontal ? rect.width : rect.height;
        }
        step = horizontal
            ? ((last.x + last.width) - first.x - occupied) /
                (double)(count - 1U)
            : ((last.y + last.height) - first.y - occupied) /
                (double)(count - 1U);
    } else {
        const double first_centre = horizontal
            ? first.x + first.width / 2.0 : first.y + first.height / 2.0;
        const double last_centre = horizontal
            ? last.x + last.width / 2.0 : last.y + last.height / 2.0;
        step = (last_centre - first_centre) / (double)(count - 1U);
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->previous_revision = document->version.revision;
    {
        double cursor = horizontal ? first.x : first.y;
        const double first_centre = horizontal
            ? first.x + first.width / 2.0 : first.y + first.height / 2.0;
        for (position = 1U; position + 1U < count; ++position) {
            UmiWorkbenchLayoutNode *node = &document->nodes[indices[position]];
            UmiWorkbenchDesignerRect rect =
                umi_workbench_designer_from_layout_rect(node->bounds);
            UmiWorkbenchLayoutRect layout_rect;
            if (gaps) {
                cursor += horizontal
                    ? umi_workbench_designer_from_layout_rect(
                        document->nodes[indices[position - 1U]].bounds).width
                    : umi_workbench_designer_from_layout_rect(
                        document->nodes[indices[position - 1U]].bounds).height;
                cursor += step;
                if (horizontal) rect.x = cursor; else rect.y = cursor;
            } else {
                const double centre = first_centre + step * (double)position;
                if (horizontal) rect.x = centre - rect.width / 2.0;
                else rect.y = centre - rect.height / 2.0;
            }
            layout_rect = umi_workbench_designer_to_layout_rect(rect);
            if (umi_workbench_layout_node_set_bounds(node, &layout_rect) ==
                UMI_STATUS_OK) {
                node->revision += 1U;
                out_result->changed_count += 1U;
            }
        }
    }
    if (out_result->changed_count > 0U) {
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
    }
    out_result->resulting_bounds = alignment_bounds(document, selection);
    out_result->resulting_revision = document->version.revision;
    return UMI_STATUS_OK;
}
