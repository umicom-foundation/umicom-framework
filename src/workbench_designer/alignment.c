/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/alignment.c
 *
 * PURPOSE:
 *   Apply alignment and distribution to explicit layout bounds while preserving
 *   stable node identities and revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/alignment.h"
#include "internal.h"
#include <math.h>


/* Find alignment while leaving the underlying catalogue or model owned by this module. */
static bool alignment_selected(
    const UmiWorkbenchDesignerSelection *selection,
    const UmiWorkbenchLayoutNode *node)
{
    return selection != NULL && node != NULL &&
        umi_workbench_designer_selection_contains(selection, node->node_id);
}

/* Provide the alignment bounds operation used by this module and its client applications. */
static UmiWorkbenchDesignerRect alignment_bounds(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection)
{
    UmiWorkbenchDesignerRect result = {0.0, 0.0, 0.0, 0.0};
    bool first = true;
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        const UmiWorkbenchLayoutNode *node = &document->nodes[index];
        UmiWorkbenchDesignerRect rect;
        double right;
        double bottom;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!alignment_selected(selection, node)) continue;
        rect = umi_workbench_designer_from_layout_rect(node->bounds);
        /* Apply this operation only while the related capability or state is available. */
        if (!umi_workbench_designer_rect_is_valid(&rect)) continue;
        /* Apply this branch only when its contract condition is satisfied. */
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

/* Provide the alignment target operation used by this module and its client applications. */
static double alignment_target(
    UmiWorkbenchDesignerAlignment alignment,
    UmiWorkbenchDesignerRect bounds)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the workbench designer align selection operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_align_selection(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    UmiWorkbenchDesignerAlignment alignment,
    UmiWorkbenchDesignerAlignmentResult *out_result)
{
    UmiWorkbenchDesignerRect bounds;
    double target;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || selection == NULL || out_result == NULL ||
        selection->count < 2U || alignment < UMI_WORKBENCH_DESIGNER_ALIGN_LEFT ||
        alignment > UMI_WORKBENCH_DESIGNER_ALIGN_BOTTOM) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_document_has_flag(
            document, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->previous_revision = document->version.revision;
    bounds = alignment_bounds(document, selection);
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_designer_rect_is_valid(&bounds)) {
        return UMI_STATUS_INVALID_STATE;
    }
    target = alignment_target(alignment, bounds);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        UmiWorkbenchLayoutNode *node = &document->nodes[index];
        UmiWorkbenchDesignerRect rect;
        UmiWorkbenchLayoutRect layout_rect;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!alignment_selected(selection, node)) continue;
        rect = umi_workbench_designer_from_layout_rect(node->bounds);
        /* Select the behaviour associated with the requested command or state value. */
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
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_workbench_layout_node_set_bounds(node, &layout_rect) ==
            UMI_STATUS_OK) {
            node->revision += 1U;
            out_result->changed_count += 1U;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_result->changed_count > 0U) {
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
    }
    out_result->resulting_bounds = alignment_bounds(document, selection);
    out_result->resulting_revision = document->version.revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the alignment sort indices operation used by this module and its client
 * applications.
 */
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
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (alignment_selected(selection, &document->nodes[index])) {
            indices[count++] = index;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (left = 1U; left < count; ++left) {
        const size_t value = indices[left];
        const UmiWorkbenchDesignerRect value_rect =
            umi_workbench_designer_from_layout_rect(document->nodes[value].bounds);
        const double value_key = horizontal ? value_rect.x : value_rect.y;
        right = left;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (right > 0U) {
            const UmiWorkbenchDesignerRect previous_rect =
                umi_workbench_designer_from_layout_rect(
                    document->nodes[indices[right - 1U]].bounds);
            const double previous_key = horizontal
                ? previous_rect.x : previous_rect.y;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (previous_key <= value_key) break;
            indices[right] = indices[right - 1U];
            --right;
        }
        indices[right] = value;
    }
    *out_count = count;
}

/*
 * Provide the workbench designer distribute selection operation used by this module and
 * its client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || selection == NULL || out_result == NULL ||
        selection->count < 3U ||
        distribution < UMI_WORKBENCH_DESIGNER_DISTRIBUTE_HORIZONTAL_CENTRES ||
        distribution > UMI_WORKBENCH_DESIGNER_DISTRIBUTE_VERTICAL_GAPS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count < 3U) return UMI_STATUS_INVALID_STATE;
    first = umi_workbench_designer_from_layout_rect(
        document->nodes[indices[0U]].bounds);
    last = umi_workbench_designer_from_layout_rect(
        document->nodes[indices[count - 1U]].bounds);
    /* Apply this branch only when its contract condition is satisfied. */
    if (gaps) {
        /* Visit each bounded item once so every record receives the same rule. */
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
    } /* Use this fallback path when the earlier condition does not apply. */ else {
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
        /* Visit each bounded item once so every record receives the same rule. */
        for (position = 1U; position + 1U < count; ++position) {
            UmiWorkbenchLayoutNode *node = &document->nodes[indices[position]];
            UmiWorkbenchDesignerRect rect =
                umi_workbench_designer_from_layout_rect(node->bounds);
            UmiWorkbenchLayoutRect layout_rect;
            /* Apply this branch only when its contract condition is satisfied. */
            if (gaps) {
                cursor += horizontal
                    ? umi_workbench_designer_from_layout_rect(
                        document->nodes[indices[position - 1U]].bounds).width
                    : umi_workbench_designer_from_layout_rect(
                        document->nodes[indices[position - 1U]].bounds).height;
                cursor += step;
                /* Apply this branch only when its contract condition is satisfied. */
                if (horizontal) rect.x = cursor; /* Use this fallback path when the earlier condition does not apply. */ else rect.y = cursor;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                const double centre = first_centre + step * (double)position;
                /* Apply this branch only when its contract condition is satisfied. */
                if (horizontal) rect.x = centre - rect.width / 2.0;
                /* Use this fallback path when the earlier condition does not apply. */
                else rect.y = centre - rect.height / 2.0;
            }
            layout_rect = umi_workbench_designer_to_layout_rect(rect);
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_workbench_layout_node_set_bounds(node, &layout_rect) ==
                UMI_STATUS_OK) {
                node->revision += 1U;
                out_result->changed_count += 1U;
            }
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_result->changed_count > 0U) {
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
    }
    out_result->resulting_bounds = alignment_bounds(document, selection);
    out_result->resulting_revision = document->version.revision;
    return UMI_STATUS_OK;
}
