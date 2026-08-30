/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/review.c
 *
 * PURPOSE:
 *   Build pre-save review evidence from the semantic diff engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/review.h"
#include <string.h>


UmiStatus umi_workbench_designer_review_build(
    UmiWorkbenchDesignerReview *review,
    const UmiWorkbenchLayoutDocument *baseline,
    const UmiWorkbenchLayoutDocument *current)
{
    UmiWorkbenchLayoutDiffOptions options;
    UmiStatus status;
    if (review == NULL || baseline == NULL || current == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(review, 0, sizeof(*review));
    status = umi_workbench_layout_document_copy(&review->baseline, baseline);
    if (status != UMI_STATUS_OK) return status;
    options = umi_workbench_layout_diff_options_default();
    options.include_geometry = true;
    options.include_metadata = true;
    options.include_order = true;
    status = umi_workbench_layout_compare(
        baseline, current, &options, &review->diff);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_designer_diff_view_build(&review->view, &review->diff);
    if (status != UMI_STATUS_OK) return status;
    review->metadata_change_count = review->diff.metadata_change_count;
    review->structural_change_count = review->diff.added_count +
        review->diff.removed_count + review->diff.moved_count +
        review->diff.changed_count;
    review->requires_confirmation = review->diff.removed_count > 0U ||
        review->diff.moved_count > 0U;
    review->baseline_revision = baseline->version.revision;
    review->current_revision = current->version.revision;
    review->valid = true;
    return UMI_STATUS_OK;
}
