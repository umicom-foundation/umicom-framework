/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/viewport.h
 *
 * PURPOSE:
 *   Describe pan, zoom, coordinate conversion and fit behaviour for a toolkit-
 *   neutral semantic canvas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_VIEWPORT_H
#define UMICOM_WORKBENCH_DESIGNER_VIEWPORT_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer viewport data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerViewport {
    UmiWorkbenchDesignerPoint origin;
    UmiWorkbenchDesignerSize canvas_size;
    double zoom;
    double minimum_zoom;
    double maximum_zoom;
    uint64_t revision;
} UmiWorkbenchDesignerViewport;

/**
 * Initialise workbench designer viewport from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_viewport_init(UmiWorkbenchDesignerViewport *viewport, double width, double height);
/**
 * Return the number of records represented by workbench designer viewport set canvas
 * without changing their state.
 */
UmiStatus umi_workbench_designer_viewport_set_canvas_size(UmiWorkbenchDesignerViewport *viewport, double width, double height);
/**
 * Provide the workbench designer viewport set zoom operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_viewport_set_zoom(UmiWorkbenchDesignerViewport *viewport, double zoom, UmiWorkbenchDesignerPoint anchor_screen);
/**
 * Provide the workbench designer viewport pan operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_viewport_pan(UmiWorkbenchDesignerViewport *viewport, double delta_x, double delta_y);
/**
 * Provide the workbench designer viewport world to screen operation used by this module
 * and its client applications.
 */
UmiWorkbenchDesignerPoint umi_workbench_designer_viewport_world_to_screen(const UmiWorkbenchDesignerViewport *viewport, UmiWorkbenchDesignerPoint point);
/**
 * Provide the workbench designer viewport screen to world operation used by this module
 * and its client applications.
 */
UmiWorkbenchDesignerPoint umi_workbench_designer_viewport_screen_to_world(const UmiWorkbenchDesignerViewport *viewport, UmiWorkbenchDesignerPoint point);
/**
 * Provide the workbench designer viewport fit rect operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_viewport_fit_rect(UmiWorkbenchDesignerViewport *viewport, const UmiWorkbenchDesignerRect *world_rect, double padding);
/**
 * Provide the workbench designer viewport visible world operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerRect umi_workbench_designer_viewport_visible_world(const UmiWorkbenchDesignerViewport *viewport);

#ifdef __cplusplus
}
#endif

#endif
