/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/renderer.h
 *
 * PURPOSE:
 *   Define the versioned graphics-backend contract for chart render scenes.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_RENDERER_H
#define UMICOM_CHART_RENDERER_H

#include "umicom/chart/render_scene.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_RENDERER_API_VERSION 1U

/* Report the drawing features supplied by one renderer implementation. */
typedef enum UmiChartRendererCapability {
    UMI_CHART_RENDERER_CAPABILITY_LINE = 1U << 0,
    UMI_CHART_RENDERER_CAPABILITY_RECTANGLE = 1U << 1,
    UMI_CHART_RENDERER_CAPABILITY_CIRCLE = 1U << 2,
    UMI_CHART_RENDERER_CAPABILITY_TEXT = 1U << 3
} UmiChartRendererCapability;

/*
 * Each callback draws one validated command.  The opaque context belongs to
 * the adapter, allowing a native canvas, image exporter or web renderer to
 * keep its own state without leaking that state into chart calculations.
 */
typedef struct UmiChartRendererOperations {
    UmiStatus (*begin)(void *context, double width, double height);
    UmiStatus (*draw_line)(
        void *context,
        UmiChartRenderPoint start,
        UmiChartRenderPoint end,
        UmiChartColor color,
        double stroke_width
    );
    UmiStatus (*fill_rectangle)(
        void *context,
        UmiChartRenderRectangle rectangle,
        UmiChartColor color
    );
    UmiStatus (*stroke_rectangle)(
        void *context,
        UmiChartRenderRectangle rectangle,
        UmiChartColor color,
        double stroke_width
    );
    UmiStatus (*fill_circle)(
        void *context,
        UmiChartRenderPoint centre,
        double radius,
        UmiChartColor color
    );
    UmiStatus (*draw_text)(
        void *context,
        UmiChartRenderPoint position,
        const char *text,
        UmiChartColor color
    );
    UmiStatus (*end)(void *context);
} UmiChartRendererOperations;

/* Describe one renderer and the adapter-owned context passed to its callbacks. */
typedef struct UmiChartRenderer {
    uint32_t struct_size;
    uint32_t api_version;
    uint32_t capabilities;
    void *context;
    UmiChartRendererOperations operations;
} UmiChartRenderer;

/* Check ABI fields, capabilities and required callbacks before rendering. */
UmiStatus umi_chart_renderer_validate(const UmiChartRenderer *renderer);

/* Draw every scene command in order into a target of the requested size. */
UmiStatus umi_chart_renderer_render_scene(
    const UmiChartRenderer *renderer,
    const UmiChartRenderScene *scene,
    double width,
    double height
);

#ifdef __cplusplus
}
#endif

#endif
