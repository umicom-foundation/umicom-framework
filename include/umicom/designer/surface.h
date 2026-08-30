/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/surface.h
 *
 * PURPOSE:
 *   Provide toolkit-neutral design-surface geometry, grid, zoom, alignment and
 *   distribution operations while persisting semantic component rectangles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Geometry mathematics is reusable and therefore belongs to Framework. GTK4
 * merely draws the resulting rectangles and handles pointer interaction.
 */

#ifndef UMICOM_DESIGNER_SURFACE_H
#define UMICOM_DESIGNER_SURFACE_H

#include "umicom/designer/document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerSurfaceOptions {
    unsigned grid_size;
    float zoom;
    int show_grid;
    int snap_to_grid;
    int show_guides;
} UmiDesignerSurfaceOptions;

UmiStatus umi_designer_surface_get_rect(
    const UmiDesignerDocument *document,
    const char *node_id,
    UmiDesignerRect *out_rect
);

UmiStatus umi_designer_surface_set_rect(
    UmiDesignerDocument *document,
    const char *node_id,
    UmiDesignerRect rect
);

void umi_designer_surface_options_init(UmiDesignerSurfaceOptions *options);

UmiStatus umi_designer_surface_set_zoom(
    UmiDesignerSurfaceOptions *options,
    float zoom
);

UmiStatus umi_designer_surface_set_grid(
    UmiDesignerSurfaceOptions *options,
    unsigned grid_size,
    int snap_to_grid
);

UmiDesignerRect umi_designer_surface_snap_rect(
    const UmiDesignerSurfaceOptions *options,
    UmiDesignerRect rect
);

UmiStatus umi_designer_surface_align(
    UmiDesignerRect *rects,
    size_t count,
    UmiDesignerAlignment alignment
);

UmiStatus umi_designer_surface_distribute(
    UmiDesignerRect *rects,
    size_t count,
    UmiDesignerDistribution distribution
);

#ifdef __cplusplus
}
#endif

#endif
