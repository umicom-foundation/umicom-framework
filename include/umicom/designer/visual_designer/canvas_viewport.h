/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/canvas_viewport.h
 *
 * PURPOSE:
 *   Track canvas origin, dimensions and zoom independently from document geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_CANVAS_VIEWPORT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_CANVAS_VIEWPORT_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer canvas viewport data shared with callers of this public contract.
 */
typedef struct UmiRadCanvasViewport {
    UmiRadPoint origin;
    UmiRadSize extent;
    double zoom;
} UmiRadCanvasViewport;
/**
 * Initialise visual designer canvas viewport from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_canvas_viewport_init(UmiRadCanvasViewport *item);
/**
 * Check that visual designer canvas viewport satisfies its contract before another service relies on
 * it.
 */
int umi_rad_canvas_viewport_is_valid(const UmiRadCanvasViewport *item);
#ifdef __cplusplus
}
#endif
#endif
