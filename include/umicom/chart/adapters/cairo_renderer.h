/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/adapters/cairo_renderer.h
 *
 * PURPOSE:
 *   Adapt toolkit-neutral chart render scenes to a Cairo drawing context.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_ADAPTERS_CAIRO_RENDERER_H
#define UMICOM_CHART_ADAPTERS_CAIRO_RENDERER_H

#include <cairo.h>

#include "umicom/chart/renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_CAIRO_FONT_FAMILY_CAPACITY 64U

/*
 * Keep per-frame adapter state outside the renderer contract.  The drawing
 * context remains owned by the caller and must stay alive for the render call.
 */
typedef struct UmiChartCairoRendererContext {
    cairo_t *drawing_context;
    char font_family[UMI_CHART_CAIRO_FONT_FAMILY_CAPACITY];
    double font_size;
    int frame_active;
} UmiChartCairoRendererContext;

/* Initialise a Cairo context and return a complete generic renderer descriptor. */
UmiStatus umi_chart_cairo_renderer_init(
    UmiChartCairoRendererContext *context,
    cairo_t *drawing_context,
    UmiChartRenderer *out_renderer
);

/* Change the bounded text family and size used by later scene labels. */
UmiStatus umi_chart_cairo_renderer_set_font(
    UmiChartCairoRendererContext *context,
    const char *font_family,
    double font_size
);

#ifdef __cplusplus
}
#endif

#endif
