/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/marquee.h
 *
 * PURPOSE:
 *   Represent rectangular marquee selection and apply containment or
 *   intersection policies to canvas items.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_MARQUEE_H
#define UMICOM_WORKBENCH_DESIGNER_MARQUEE_H

#include "umicom/workbench_designer/canvas.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerMarqueeMode {
    UMI_WORKBENCH_DESIGNER_MARQUEE_CONTAIN = 1,
    UMI_WORKBENCH_DESIGNER_MARQUEE_INTERSECT = 2
} UmiWorkbenchDesignerMarqueeMode;

typedef struct UmiWorkbenchDesignerMarquee {
    UmiWorkbenchDesignerPoint origin;
    UmiWorkbenchDesignerPoint current;
    UmiWorkbenchDesignerRect bounds;
    UmiWorkbenchDesignerMarqueeMode mode;
    bool active;
    bool extend_selection;
    uint64_t revision;
} UmiWorkbenchDesignerMarquee;

void umi_workbench_designer_marquee_init(UmiWorkbenchDesignerMarquee *marquee);
UmiStatus umi_workbench_designer_marquee_begin(
    UmiWorkbenchDesignerMarquee *marquee,
    UmiWorkbenchDesignerPoint origin,
    UmiWorkbenchDesignerMarqueeMode mode,
    bool extend_selection);
UmiStatus umi_workbench_designer_marquee_update(
    UmiWorkbenchDesignerMarquee *marquee,
    UmiWorkbenchDesignerPoint current);
UmiStatus umi_workbench_designer_marquee_apply(
    const UmiWorkbenchDesignerMarquee *marquee,
    const UmiWorkbenchDesignerCanvas *canvas,
    UmiWorkbenchDesignerSelection *selection);
void umi_workbench_designer_marquee_cancel(UmiWorkbenchDesignerMarquee *marquee);

#ifdef __cplusplus
}
#endif

#endif
