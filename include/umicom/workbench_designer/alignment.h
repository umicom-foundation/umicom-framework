/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/alignment.h
 *
 * PURPOSE:
 *   Provide deterministic alignment and distribution calculations for selected
 *   semantic layout nodes without depending on a frontend toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_ALIGNMENT_H
#define UMICOM_WORKBENCH_DESIGNER_ALIGNMENT_H

#include "umicom/workbench_designer/canvas.h"
#include "umicom/workbench_designer/selection.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench designer alignment values accepted by this public contract.
 */
typedef enum UmiWorkbenchDesignerAlignment {
    UMI_WORKBENCH_DESIGNER_ALIGN_LEFT = 1,
    UMI_WORKBENCH_DESIGNER_ALIGN_HORIZONTAL_CENTRE = 2,
    UMI_WORKBENCH_DESIGNER_ALIGN_RIGHT = 3,
    UMI_WORKBENCH_DESIGNER_ALIGN_TOP = 4,
    UMI_WORKBENCH_DESIGNER_ALIGN_VERTICAL_CENTRE = 5,
    UMI_WORKBENCH_DESIGNER_ALIGN_BOTTOM = 6
} UmiWorkbenchDesignerAlignment;

/**
 * List the named workbench designer distribution values accepted by this public contract.
 */
typedef enum UmiWorkbenchDesignerDistribution {
    UMI_WORKBENCH_DESIGNER_DISTRIBUTE_HORIZONTAL_CENTRES = 1,
    UMI_WORKBENCH_DESIGNER_DISTRIBUTE_HORIZONTAL_GAPS = 2,
    UMI_WORKBENCH_DESIGNER_DISTRIBUTE_VERTICAL_CENTRES = 3,
    UMI_WORKBENCH_DESIGNER_DISTRIBUTE_VERTICAL_GAPS = 4
} UmiWorkbenchDesignerDistribution;

/**
 * Represent the workbench designer alignment result data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerAlignmentResult {
    size_t changed_count;
    UmiWorkbenchDesignerRect resulting_bounds;
    uint64_t previous_revision;
    uint64_t resulting_revision;
} UmiWorkbenchDesignerAlignmentResult;

/**
 * Provide the workbench designer align selection operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_align_selection(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    UmiWorkbenchDesignerAlignment alignment,
    UmiWorkbenchDesignerAlignmentResult *out_result);
/**
 * Provide the workbench designer distribute selection operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_distribute_selection(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    UmiWorkbenchDesignerDistribution distribution,
    UmiWorkbenchDesignerAlignmentResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
