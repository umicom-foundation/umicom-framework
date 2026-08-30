/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/review.h
 *
 * PURPOSE:
 *   Compare the edited document with its persisted baseline and prepare a save
 *   review summary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_REVIEW_H
#define UMICOM_WORKBENCH_DESIGNER_REVIEW_H

#include "umicom/workbench_designer/diff_view.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerReview {
    UmiWorkbenchLayoutDocument baseline;
    UmiWorkbenchLayoutDiff diff;
    UmiWorkbenchDesignerDiffView view;
    size_t structural_change_count;
    size_t metadata_change_count;
    bool requires_confirmation;
    bool valid;
    uint64_t baseline_revision;
    uint64_t current_revision;
} UmiWorkbenchDesignerReview;

UmiStatus umi_workbench_designer_review_build(UmiWorkbenchDesignerReview *review, const UmiWorkbenchLayoutDocument *baseline, const UmiWorkbenchLayoutDocument *current);

#ifdef __cplusplus
}
#endif

#endif
