/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/alignment.h
 *
 * PURPOSE:
 *   Provide deterministic alignment and distribution calculations for selected
 *   semantic layout nodes without depending on a frontend toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_ALIGNMENT_H
#define UMICOM_WORKBENCH_DESIGNER_ALIGNMENT_H

#include "umicom/workbench_designer/canvas.h"
#include "umicom/workbench_designer/selection.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerAlignment {
    UMI_WORKBENCH_DESIGNER_ALIGN_LEFT = 1,
    UMI_WORKBENCH_DESIGNER_ALIGN_HORIZONTAL_CENTRE = 2,
    UMI_WORKBENCH_DESIGNER_ALIGN_RIGHT = 3,
    UMI_WORKBENCH_DESIGNER_ALIGN_TOP = 4,
    UMI_WORKBENCH_DESIGNER_ALIGN_VERTICAL_CENTRE = 5,
    UMI_WORKBENCH_DESIGNER_ALIGN_BOTTOM = 6
} UmiWorkbenchDesignerAlignment;

typedef enum UmiWorkbenchDesignerDistribution {
    UMI_WORKBENCH_DESIGNER_DISTRIBUTE_HORIZONTAL_CENTRES = 1,
    UMI_WORKBENCH_DESIGNER_DISTRIBUTE_HORIZONTAL_GAPS = 2,
    UMI_WORKBENCH_DESIGNER_DISTRIBUTE_VERTICAL_CENTRES = 3,
    UMI_WORKBENCH_DESIGNER_DISTRIBUTE_VERTICAL_GAPS = 4
} UmiWorkbenchDesignerDistribution;

typedef struct UmiWorkbenchDesignerAlignmentResult {
    size_t changed_count;
    UmiWorkbenchDesignerRect resulting_bounds;
    uint64_t previous_revision;
    uint64_t resulting_revision;
} UmiWorkbenchDesignerAlignmentResult;

UmiStatus umi_workbench_designer_align_selection(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    UmiWorkbenchDesignerAlignment alignment,
    UmiWorkbenchDesignerAlignmentResult *out_result);
UmiStatus umi_workbench_designer_distribute_selection(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    UmiWorkbenchDesignerDistribution distribution,
    UmiWorkbenchDesignerAlignmentResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
