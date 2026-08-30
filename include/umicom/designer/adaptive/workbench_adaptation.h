/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/workbench_adaptation.h
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

#ifndef UMICOM_DESIGNER_ADAPTIVE_WORKBENCH_ADAPTATION_H
#define UMICOM_DESIGNER_ADAPTIVE_WORKBENCH_ADAPTATION_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerWorkbenchAdaptation {
    int left_inline;
    int right_inline;
    int bottom_inline;
    int auxiliary_overlay;
} UmiDesignerWorkbenchAdaptation;
/* Resolve high-level workbench region presentation for a canonical size class. */
UmiDesignerWorkbenchAdaptation umi_designer_workbench_adaptation_resolve(
    UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
