/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/canvas_surface.h
 *
 * PURPOSE:
 *   Define reusable zoomable/snap-capable design canvas state for GUI, kitchen, image, scene and layout designers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_CANVAS_SURFACE_H
#define UMICOM_UI_WORKSTATION_CANVAS_SURFACE_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws canvas surface data shared with callers of this public contract.
 */
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

/**
 * Initialise ws canvas surface from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_canvas_surface_init(UmiWsCanvasSurface *surface, const char *canvas_id);
/**
 * Provide the ws canvas surface set zoom operation used by this module and its client
 * applications.
 */
double umi_ws_canvas_surface_set_zoom(UmiWsCanvasSurface *surface, double zoom);
/**
 * Provide the ws canvas surface pan operation used by this module and its client
 * applications.
 */
void umi_ws_canvas_surface_pan(UmiWsCanvasSurface *surface, double delta_x, double delta_y);
/**
 * Provide the ws canvas surface set grid operation used by this module and its client
 * applications.
 */
void umi_ws_canvas_surface_set_grid(UmiWsCanvasSurface *surface, int32_t grid_size, bool snap);

#ifdef __cplusplus
}
#endif

#endif
