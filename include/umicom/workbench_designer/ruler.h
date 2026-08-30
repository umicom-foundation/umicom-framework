/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/ruler.h
 *
 * PURPOSE:
 *   Build horizontal and vertical ruler ticks for the semantic canvas at
 *   arbitrary zoom and pan positions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_RULER_H
#define UMICOM_WORKBENCH_DESIGNER_RULER_H

#include "umicom/workbench_designer/viewport.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_RULER_TICKS 256U

typedef enum UmiWorkbenchDesignerRulerAxis {
    UMI_WORKBENCH_DESIGNER_RULER_HORIZONTAL = 1,
    UMI_WORKBENCH_DESIGNER_RULER_VERTICAL = 2
} UmiWorkbenchDesignerRulerAxis;

typedef struct UmiWorkbenchDesignerRulerTick {
    double world_value;
    double screen_position;
    bool major;
    char label[32];
} UmiWorkbenchDesignerRulerTick;

typedef struct UmiWorkbenchDesignerRuler {
    UmiWorkbenchDesignerRulerAxis axis;
    UmiWorkbenchDesignerRulerTick ticks[UMI_WORKBENCH_DESIGNER_MAX_RULER_TICKS];
    size_t count;
    double minor_step;
    double major_step;
    uint64_t viewport_revision;
    uint64_t revision;
} UmiWorkbenchDesignerRuler;

void umi_workbench_designer_ruler_init(
    UmiWorkbenchDesignerRuler *ruler,
    UmiWorkbenchDesignerRulerAxis axis);
UmiStatus umi_workbench_designer_ruler_build(
    UmiWorkbenchDesignerRuler *ruler,
    const UmiWorkbenchDesignerViewport *viewport,
    double screen_extent,
    double preferred_minor_pixels);
const UmiWorkbenchDesignerRulerTick *umi_workbench_designer_ruler_at(
    const UmiWorkbenchDesignerRuler *ruler,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
