/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/workbench_adaptation.c
 *
 * PURPOSE:
 *   Plan adaptive workbench side/bottom regions for desktop, tablet and handset previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/workbench_adaptation.h"

/* Preserve centre content while progressively moving auxiliary workbench regions to overlays. */
UmiDesignerWorkbenchAdaptation umi_designer_workbench_adaptation_resolve(
    UmiDesignSizeClass size_class)
{
    UmiDesignerWorkbenchAdaptation result = {0,0,0,0};
    if (size_class == UMI_DESIGN_SIZE_WIDE) {
        result.left_inline = 1; result.right_inline = 1; result.bottom_inline = 1;
    } else if (size_class == UMI_DESIGN_SIZE_EXPANDED) {
        result.left_inline = 1; result.bottom_inline = 1; result.auxiliary_overlay = 1;
    } else if (size_class == UMI_DESIGN_SIZE_MEDIUM) {
        result.left_inline = 1; result.auxiliary_overlay = 1;
    } else {
        result.auxiliary_overlay = 1;
    }
    return result;
}
