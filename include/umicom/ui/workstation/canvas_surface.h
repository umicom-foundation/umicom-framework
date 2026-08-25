/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/canvas_surface.h
 *
 * PURPOSE:
 *   Define reusable zoomable/snap-capable design canvas state for GUI, kitchen, image, scene and layout designers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_CANVAS_SURFACE_H
#define UMICOM_UI_WORKSTATION_CANVAS_SURFACE_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsCanvasSurface {
    char canvas_id[UMI_UI_ID_CAPACITY];
    double zoom;
    double pan_x;
    double pan_y;
    int32_t grid_size;
    bool show_grid;
    bool snap_to_grid;
    bool show_rulers;
} UmiWsCanvasSurface;

UmiStatus umi_ws_canvas_surface_init(UmiWsCanvasSurface *surface, const char *canvas_id);
double umi_ws_canvas_surface_set_zoom(UmiWsCanvasSurface *surface, double zoom);
void umi_ws_canvas_surface_pan(UmiWsCanvasSurface *surface, double delta_x, double delta_y);
void umi_ws_canvas_surface_set_grid(UmiWsCanvasSurface *surface, int32_t grid_size, bool snap);

#ifdef __cplusplus
}
#endif

#endif
