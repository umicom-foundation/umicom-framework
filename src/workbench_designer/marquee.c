/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/marquee.c
 *
 * PURPOSE:
 *   Convert pointer movement into a normalised selection rectangle and update
 *   semantic selection state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/marquee.h"
#include "internal.h"
#include <math.h>


/* Provide the marquee bounds operation used by this module and its client applications. */
static UmiWorkbenchDesignerRect marquee_bounds(
    UmiWorkbenchDesignerPoint origin,
    UmiWorkbenchDesignerPoint current)
{
    UmiWorkbenchDesignerRect bounds;
    bounds.x = fmin(origin.x, current.x);
    bounds.y = fmin(origin.y, current.y);
    bounds.width = fabs(current.x - origin.x);
    bounds.height = fabs(current.y - origin.y);
    return bounds;
}

/*
 * Initialise workbench designer marquee from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_marquee_init(UmiWorkbenchDesignerMarquee *marquee)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (marquee == NULL) return;
    (void)memset(marquee, 0, sizeof(*marquee));
    marquee->mode = UMI_WORKBENCH_DESIGNER_MARQUEE_INTERSECT;
}

/*
 * Provide the workbench designer marquee begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_marquee_begin(
    UmiWorkbenchDesignerMarquee *marquee,
    UmiWorkbenchDesignerPoint origin,
    UmiWorkbenchDesignerMarqueeMode mode,
    bool extend_selection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (marquee == NULL ||
        (mode != UMI_WORKBENCH_DESIGNER_MARQUEE_CONTAIN &&
         mode != UMI_WORKBENCH_DESIGNER_MARQUEE_INTERSECT)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_marquee_init(marquee);
    marquee->origin = origin;
    marquee->current = origin;
    marquee->bounds = marquee_bounds(origin, origin);
    marquee->mode = mode;
    marquee->extend_selection = extend_selection;
    marquee->active = true;
    marquee->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer marquee update operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_marquee_update(
    UmiWorkbenchDesignerMarquee *marquee,
    UmiWorkbenchDesignerPoint current)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (marquee == NULL || !marquee->active) return UMI_STATUS_INVALID_STATE;
    marquee->current = current;
    marquee->bounds = marquee_bounds(marquee->origin, current);
    marquee->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the marquee contains rect operation used by this module and its client
 * applications.
 */
static bool marquee_contains_rect(
    UmiWorkbenchDesignerRect outer,
    UmiWorkbenchDesignerRect inner)
{
    return inner.x >= outer.x && inner.y >= outer.y &&
        inner.x + inner.width <= outer.x + outer.width &&
        inner.y + inner.height <= outer.y + outer.height;
}

/*
 * Perform workbench designer marquee through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_workbench_designer_marquee_apply(
    const UmiWorkbenchDesignerMarquee *marquee,
    const UmiWorkbenchDesignerCanvas *canvas,
    UmiWorkbenchDesignerSelection *selection)
{
    size_t index;
    size_t selected_count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (marquee == NULL || canvas == NULL || selection == NULL ||
        !marquee->active) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!marquee->extend_selection) {
        umi_workbench_designer_selection_clear(selection);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < canvas->count; ++index) {
        const UmiWorkbenchDesignerCanvasItem *item = &canvas->items[index];
        bool match;
        /* Apply this operation only while the related capability or state is available. */
        if (!item->visible || item->container) continue;
        match = marquee->mode == UMI_WORKBENCH_DESIGNER_MARQUEE_CONTAIN
            ? marquee_contains_rect(marquee->bounds, item->screen_bounds)
            : umi_workbench_designer_rect_intersects(
                &marquee->bounds, &item->screen_bounds);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!match) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_workbench_designer_selection_add(
                selection, item->node_id, selected_count == 0U) ==
            UMI_STATUS_OK) {
            selected_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer marquee cancel operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_marquee_cancel(UmiWorkbenchDesignerMarquee *marquee)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (marquee == NULL) return;
    marquee->active = false;
    marquee->revision += 1U;
}
