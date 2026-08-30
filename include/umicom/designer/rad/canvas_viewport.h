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
typedef struct UmiRadCanvasViewport {
    UmiRadPoint origin;
    UmiRadSize extent;
    double zoom;
} UmiRadCanvasViewport;
UmiStatus umi_rad_canvas_viewport_init(UmiRadCanvasViewport *item);
int umi_rad_canvas_viewport_is_valid(const UmiRadCanvasViewport *item);
#ifdef __cplusplus
}
#endif
#endif
