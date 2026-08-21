/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/viewport.h
 *
 * PURPOSE:
 *   Describe pan, zoom, coordinate conversion and fit behaviour for a toolkit-
 *   neutral semantic canvas.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_VIEWPORT_H
#define UMICOM_WORKBENCH_DESIGNER_VIEWPORT_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerViewport {
    UmiWorkbenchDesignerPoint origin;
    UmiWorkbenchDesignerSize canvas_size;
    double zoom;
    double minimum_zoom;
    double maximum_zoom;
    uint64_t revision;
} UmiWorkbenchDesignerViewport;

void umi_workbench_designer_viewport_init(UmiWorkbenchDesignerViewport *viewport, double width, double height);
UmiStatus umi_workbench_designer_viewport_set_canvas_size(UmiWorkbenchDesignerViewport *viewport, double width, double height);
UmiStatus umi_workbench_designer_viewport_set_zoom(UmiWorkbenchDesignerViewport *viewport, double zoom, UmiWorkbenchDesignerPoint anchor_screen);
UmiStatus umi_workbench_designer_viewport_pan(UmiWorkbenchDesignerViewport *viewport, double delta_x, double delta_y);
UmiWorkbenchDesignerPoint umi_workbench_designer_viewport_world_to_screen(const UmiWorkbenchDesignerViewport *viewport, UmiWorkbenchDesignerPoint point);
UmiWorkbenchDesignerPoint umi_workbench_designer_viewport_screen_to_world(const UmiWorkbenchDesignerViewport *viewport, UmiWorkbenchDesignerPoint point);
UmiStatus umi_workbench_designer_viewport_fit_rect(UmiWorkbenchDesignerViewport *viewport, const UmiWorkbenchDesignerRect *world_rect, double padding);
UmiWorkbenchDesignerRect umi_workbench_designer_viewport_visible_world(const UmiWorkbenchDesignerViewport *viewport);

#ifdef __cplusplus
}
#endif

#endif
