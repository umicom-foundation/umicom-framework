/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/canvas_surface.c
 *
 * PURPOSE:
 *   Implement reusable zoomable/snap-capable design canvas state for GUI, kitchen, image, scene and layout designers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/canvas_surface.h"

/*
 * Initialise ws canvas surface from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_canvas_surface_init(UmiWsCanvasSurface *surface, const char *canvas_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || !umi_ws_id_valid(canvas_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *surface = (UmiWsCanvasSurface){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(surface->canvas_id, sizeof(surface->canvas_id), canvas_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    surface->zoom = 1.0;
    surface->grid_size = 16;
    surface->show_grid = true;
    surface->snap_to_grid = true;
    surface->show_rulers = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws canvas surface set zoom operation used by this module and its client
 * applications.
 */
double umi_ws_canvas_surface_set_zoom(UmiWsCanvasSurface *surface, double zoom) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL) return 0.0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (zoom < 0.05) zoom = 0.05;
    /* Apply this branch only when its contract condition is satisfied. */
    if (zoom > 64.0) zoom = 64.0;
    surface->zoom = zoom;
    return zoom;
}

/*
 * Provide the ws canvas surface pan operation used by this module and its client
 * applications.
 */
void umi_ws_canvas_surface_pan(UmiWsCanvasSurface *surface, double delta_x, double delta_y) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface != NULL) {
        surface->pan_x += delta_x;
        surface->pan_y += delta_y;
    }
}

/*
 * Provide the ws canvas surface set grid operation used by this module and its client
 * applications.
 */
void umi_ws_canvas_surface_set_grid(UmiWsCanvasSurface *surface, int32_t grid_size, bool snap) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface != NULL && grid_size > 0) {
        surface->grid_size = grid_size;
        surface->snap_to_grid = snap;
    }
}
