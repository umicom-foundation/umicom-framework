/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/canvas_viewport.h
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
#ifndef UMICOM_DESIGNER_RAD_CANVAS_VIEWPORT_H
#define UMICOM_DESIGNER_RAD_CANVAS_VIEWPORT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad canvas viewport data shared with callers of this public contract.
 */
typedef struct UmiRadCanvasViewport {
    UmiRadPoint origin;
    UmiRadSize extent;
    double zoom;
} UmiRadCanvasViewport;
/**
 * Initialise rad canvas viewport from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_canvas_viewport_init(UmiRadCanvasViewport *item);
/**
 * Check that rad canvas viewport satisfies its contract before another service relies on
 * it.
 */
int umi_rad_canvas_viewport_is_valid(const UmiRadCanvasViewport *item);
#ifdef __cplusplus
}
#endif
#endif
