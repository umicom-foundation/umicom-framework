/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/alignment_guides.c
 *
 * PURPOSE:
 *   Find the nearest semantic alignment candidates and expose bounded guide
 *   segments for any frontend canvas renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/alignment_guides.h"
#include "internal.h"
#include <math.h>


/*
 * Initialise workbench designer alignment guides from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_alignment_guides_init(
    UmiWorkbenchDesignerAlignmentGuideModel *model,
    double threshold)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->threshold = threshold > 0.0 ? threshold : 6.0;
}

/* Add alignment only after its inputs and available capacity have been checked. */
static UmiStatus alignment_add(
    UmiWorkbenchDesignerAlignmentGuideModel *model,
    UmiWorkbenchDesignerAlignmentGuideKind kind,
    const char *source_node_id,
    double position,
    double start,
    double end,
    double distance,
    bool vertical)
{
    UmiWorkbenchDesignerAlignmentGuide *guide;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_WORKBENCH_DESIGNER_MAX_GUIDES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    guide = &model->guides[model->count++];
    (void)memset(guide, 0, sizeof(*guide));
    guide->kind = kind;
    (void)umi_workbench_designer_copy_text(
        guide->source_node_id, sizeof(guide->source_node_id), source_node_id);
    guide->position = position;
    guide->start = start;
    guide->end = end;
    guide->distance = distance;
    guide->vertical = vertical;
    return UMI_STATUS_OK;
}

/*
 * Provide the alignment compare axis operation used by this module and its client
 * applications.
 */
static UmiStatus alignment_compare_axis(
    UmiWorkbenchDesignerAlignmentGuideModel *model,
    const UmiWorkbenchDesignerCanvasItem *item,
    UmiWorkbenchDesignerRect moving,
    bool vertical)
{
    const double moving_points[3] = {
        vertical ? moving.x : moving.y,
        vertical ? moving.x + moving.width * 0.5 : moving.y + moving.height * 0.5,
        vertical ? moving.x + moving.width : moving.y + moving.height
    };
    const double item_points[3] = {
        vertical ? item->world_bounds.x : item->world_bounds.y,
        vertical ? item->world_bounds.x + item->world_bounds.width * 0.5
                 : item->world_bounds.y + item->world_bounds.height * 0.5,
        vertical ? item->world_bounds.x + item->world_bounds.width
                 : item->world_bounds.y + item->world_bounds.height
    };
    size_t moving_index;
    size_t item_index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (moving_index = 0U; moving_index < 3U; ++moving_index) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (item_index = 0U; item_index < 3U; ++item_index) {
            const double distance = fabs(moving_points[moving_index] - item_points[item_index]);
            UmiWorkbenchDesignerAlignmentGuideKind kind;
            double start;
            double end;
            UmiStatus status;
            /* Apply this branch only when its contract condition is satisfied. */
            if (distance > model->threshold) continue;
            /* Apply this branch only when its contract condition is satisfied. */
            if (vertical) {
                kind = item_index == 0U ? UMI_WORKBENCH_DESIGNER_GUIDE_LEFT
                    : item_index == 1U ? UMI_WORKBENCH_DESIGNER_GUIDE_HORIZONTAL_CENTRE
                    : UMI_WORKBENCH_DESIGNER_GUIDE_RIGHT;
                start = moving.y < item->world_bounds.y
                    ? moving.y : item->world_bounds.y;
                end = (moving.y + moving.height) >
                        (item->world_bounds.y + item->world_bounds.height)
                    ? moving.y + moving.height
                    : item->world_bounds.y + item->world_bounds.height;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                kind = item_index == 0U ? UMI_WORKBENCH_DESIGNER_GUIDE_TOP
                    : item_index == 1U ? UMI_WORKBENCH_DESIGNER_GUIDE_VERTICAL_CENTRE
                    : UMI_WORKBENCH_DESIGNER_GUIDE_BOTTOM;
                start = moving.x < item->world_bounds.x
                    ? moving.x : item->world_bounds.x;
                end = (moving.x + moving.width) >
                        (item->world_bounds.x + item->world_bounds.width)
                    ? moving.x + moving.width
                    : item->world_bounds.x + item->world_bounds.width;
            }
            status = alignment_add(
                model, kind, item->node_id, item_points[item_index],
                start, end, distance, vertical);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer alignment guides build operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_alignment_guides_build(
    UmiWorkbenchDesignerAlignmentGuideModel *model,
    const UmiWorkbenchDesignerCanvas *canvas,
    const char *moving_node_id,
    UmiWorkbenchDesignerRect moving_bounds)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || canvas == NULL || moving_node_id == NULL ||
        !umi_workbench_designer_rect_is_valid(&moving_bounds)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model->count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < canvas->count; ++index) {
        const UmiWorkbenchDesignerCanvasItem *item = &canvas->items[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!item->visible || strcmp(item->node_id, moving_node_id) == 0) continue;
        status = alignment_compare_axis(model, item, moving_bounds, true);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = alignment_compare_axis(model, item, moving_bounds, false);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    model->canvas_revision = canvas->revision;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find workbench designer alignment guide while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerAlignmentGuide *umi_workbench_designer_alignment_guide_at(
    const UmiWorkbenchDesignerAlignmentGuideModel *model,
    size_t index)
{
    return model != NULL && index < model->count ? &model->guides[index] : NULL;
}
