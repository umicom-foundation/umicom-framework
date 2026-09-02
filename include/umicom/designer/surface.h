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

/*
 * Geometry mathematics is reusable and therefore belongs to Framework. GTK4
 * merely draws the resulting rectangles and handles pointer interaction.
 */

#ifndef UMICOM_DESIGNER_SURFACE_H
#define UMICOM_DESIGNER_SURFACE_H

#include "umicom/designer/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer surface options data shared with callers of this public contract.
 */
typedef struct UmiDesignerSurfaceOptions {
    unsigned grid_size;
    float zoom;
    int show_grid;
    int snap_to_grid;
    int show_guides;
} UmiDesignerSurfaceOptions;

/**
 * Provide the designer surface get rect operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_surface_get_rect(
    const UmiDesignerDocument *document,
    const char *node_id,
    UmiDesignerRect *out_rect
);

/**
 * Provide the designer surface set rect operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_surface_set_rect(
    UmiDesignerDocument *document,
    const char *node_id,
    UmiDesignerRect rect
);

/**
 * Initialise designer surface options from caller-provided values so later operations
 * receive a known state.
 */
void umi_designer_surface_options_init(UmiDesignerSurfaceOptions *options);

/**
 * Provide the designer surface set zoom operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_surface_set_zoom(
    UmiDesignerSurfaceOptions *options,
    float zoom
);

/**
 * Provide the designer surface set grid operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_surface_set_grid(
    UmiDesignerSurfaceOptions *options,
    unsigned grid_size,
    int snap_to_grid
);

/**
 * Provide the designer surface snap rect operation used by this module and its client
 * applications.
 */
UmiDesignerRect umi_designer_surface_snap_rect(
    const UmiDesignerSurfaceOptions *options,
    UmiDesignerRect rect
);

/**
 * Provide the designer surface align operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_surface_align(
    UmiDesignerRect *rects,
    size_t count,
    UmiDesignerAlignment alignment
);

/**
 * Provide the designer surface distribute operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_surface_distribute(
    UmiDesignerRect *rects,
    size_t count,
    UmiDesignerDistribution distribution
);

#ifdef __cplusplus
}
#endif

#endif
