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

/**
 * List the named workbench designer ruler axis values accepted by this public contract.
 */
typedef enum UmiWorkbenchDesignerRulerAxis {
    UMI_WORKBENCH_DESIGNER_RULER_HORIZONTAL = 1,
    UMI_WORKBENCH_DESIGNER_RULER_VERTICAL = 2
} UmiWorkbenchDesignerRulerAxis;

/**
 * Represent the workbench designer ruler tick data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerRulerTick {
    double world_value;
    double screen_position;
    bool major;
    char label[32];
} UmiWorkbenchDesignerRulerTick;

/**
 * Represent the workbench designer ruler data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchDesignerRuler {
    UmiWorkbenchDesignerRulerAxis axis;
    UmiWorkbenchDesignerRulerTick ticks[UMI_WORKBENCH_DESIGNER_MAX_RULER_TICKS];
    size_t count;
    double minor_step;
    double major_step;
    uint64_t viewport_revision;
    uint64_t revision;
} UmiWorkbenchDesignerRuler;

/**
 * Initialise workbench designer ruler from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_ruler_init(
    UmiWorkbenchDesignerRuler *ruler,
    UmiWorkbenchDesignerRulerAxis axis);
/**
 * Provide the workbench designer ruler build operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_ruler_build(
    UmiWorkbenchDesignerRuler *ruler,
    const UmiWorkbenchDesignerViewport *viewport,
    double screen_extent,
    double preferred_minor_pixels);
/**
 * Find workbench designer ruler while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerRulerTick *umi_workbench_designer_ruler_at(
    const UmiWorkbenchDesignerRuler *ruler,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
