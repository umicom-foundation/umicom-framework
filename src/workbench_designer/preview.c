/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/preview.c
 *
 * PURPOSE:
 *   Build preview copies and canvas projections without mutating the edited
 *   document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/preview.h"
#include "internal.h"


/*
 * Provide the workbench designer preview build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_preview_build(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerResponsiveProfile *profile,
    UmiWorkbenchDesignerPreviewSnapshot *out_snapshot)
{
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerRect root_bounds;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || profile == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    status = umi_workbench_designer_responsive_preview_apply(
        profile, document, &out_snapshot->document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->profile = *profile;
    out_snapshot->viewport_size = profile->viewport_size;
    out_snapshot->source_revision = document->version.revision;
    out_snapshot->content_hash = document->content_hash;
    umi_workbench_designer_viewport_init(
        &viewport, profile->viewport_size.width, profile->viewport_size.height);
    umi_workbench_designer_selection_init(&selection);
    root_bounds.x = 0.0;
    root_bounds.y = 0.0;
    root_bounds.width = profile->viewport_size.width;
    root_bounds.height = profile->viewport_size.height;
    status = umi_workbench_designer_canvas_build(
        &out_snapshot->canvas, &out_snapshot->document,
        &viewport, &selection, root_bounds);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_snapshot->canvas.count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_snapshot->canvas.items[index].visible &&
            out_snapshot->canvas.items[index].world_bounds.width > 0.0 &&
            out_snapshot->canvas.items[index].world_bounds.height > 0.0) {
            out_snapshot->visible_item_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_snapshot->hidden_item_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer preview item operation used by this module and its client
 * applications.
 */
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_preview_item(
    const UmiWorkbenchDesignerPreviewSnapshot *snapshot,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (snapshot == NULL) return NULL;
    return umi_workbench_designer_canvas_at(&snapshot->canvas, index);
}
