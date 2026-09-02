/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/cairo/chart_renderer_cairo.c
 *
 * PURPOSE:
 *   Draw generic chart commands with the optional Cairo graphics adapter.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/adapters/cairo_renderer.h"

#include <math.h>
#include <string.h>

/* Translate the graphics library's sticky status into a stable Framework status. */
static UmiStatus cairo_result(const UmiChartCairoRendererContext *context)
{
    /* A missing context or drawing target is an application contract error. */
    if (context == NULL || context->drawing_context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return cairo_status(context->drawing_context) == CAIRO_STATUS_SUCCESS
               ? UMI_STATUS_OK
               : UMI_STATUS_INTERNAL_ERROR;
}

/* Apply one validated scene colour to the active drawing context. */
static void set_color(cairo_t *drawing_context, UmiChartColor color)
{
    cairo_set_source_rgba(
        drawing_context,
        color.red,
        color.green,
        color.blue,
        color.alpha);
}

/* Save the caller's graphics state and clip all commands to the target. */
static UmiStatus begin_frame(void *opaque_context, double width, double height)
{
    UmiChartCairoRendererContext *context =
        (UmiChartCairoRendererContext *)opaque_context;
    UmiStatus status;

    /* A valid drawing target and one balanced frame are required. */
    if (context == NULL || context->drawing_context == NULL ||
        context->frame_active || !isfinite(width) || !isfinite(height) ||
        width <= 0.0 || height <= 0.0) {
        return UMI_STATUS_INVALID_STATE;
    }

    cairo_save(context->drawing_context);
    cairo_rectangle(context->drawing_context, 0.0, 0.0, width, height);
    cairo_clip(context->drawing_context);
    context->frame_active = 1;
    status = cairo_result(context);
    /* Restore immediately when begin fails so the caller's save stack stays balanced. */
    if (status != UMI_STATUS_OK) {
        cairo_restore(context->drawing_context);
        context->frame_active = 0;
    }
    return status;
}

/* Draw one line command with the scene's colour and stroke width. */
static UmiStatus draw_line(
    void *opaque_context,
    UmiChartRenderPoint start,
    UmiChartRenderPoint end,
    UmiChartColor color,
    double stroke_width)
{
    UmiChartCairoRendererContext *context =
        (UmiChartCairoRendererContext *)opaque_context;

    /* Drawing outside an active frame would corrupt the caller's graphics state. */
    if (context == NULL || context->drawing_context == NULL ||
        !context->frame_active) {
        return UMI_STATUS_INVALID_STATE;
    }

    set_color(context->drawing_context, color);
    cairo_set_line_width(context->drawing_context, stroke_width);
    cairo_move_to(context->drawing_context, start.x, start.y);
    cairo_line_to(context->drawing_context, end.x, end.y);
    cairo_stroke(context->drawing_context);
    return cairo_result(context);
}

/* Draw one solid rectangle for a plot background, bar or candle body. */
static UmiStatus fill_rectangle(
    void *opaque_context,
    UmiChartRenderRectangle rectangle,
    UmiChartColor color)
{
    UmiChartCairoRendererContext *context =
        (UmiChartCairoRendererContext *)opaque_context;

    /* Drawing outside an active frame would corrupt the caller's graphics state. */
    if (context == NULL || context->drawing_context == NULL ||
        !context->frame_active) {
        return UMI_STATUS_INVALID_STATE;
    }

    set_color(context->drawing_context, color);
    cairo_rectangle(
        context->drawing_context,
        rectangle.x,
        rectangle.y,
        rectangle.width,
        rectangle.height);
    cairo_fill(context->drawing_context);
    return cairo_result(context);
}

/* Draw one rectangle outline for a plot border or selection. */
static UmiStatus stroke_rectangle(
    void *opaque_context,
    UmiChartRenderRectangle rectangle,
    UmiChartColor color,
    double stroke_width)
{
    UmiChartCairoRendererContext *context =
        (UmiChartCairoRendererContext *)opaque_context;

    /* Drawing outside an active frame would corrupt the caller's graphics state. */
    if (context == NULL || context->drawing_context == NULL ||
        !context->frame_active) {
        return UMI_STATUS_INVALID_STATE;
    }

    set_color(context->drawing_context, color);
    cairo_set_line_width(context->drawing_context, stroke_width);
    cairo_rectangle(
        context->drawing_context,
        rectangle.x,
        rectangle.y,
        rectangle.width,
        rectangle.height);
    cairo_stroke(context->drawing_context);
    return cairo_result(context);
}

/* Draw one circular marker for a scatter plot or selected value. */
static UmiStatus fill_circle(
    void *opaque_context,
    UmiChartRenderPoint centre,
    double radius,
    UmiChartColor color)
{
    UmiChartCairoRendererContext *context =
        (UmiChartCairoRendererContext *)opaque_context;

    /* Drawing outside an active frame would corrupt the caller's graphics state. */
    if (context == NULL || context->drawing_context == NULL ||
        !context->frame_active) {
        return UMI_STATUS_INVALID_STATE;
    }

    set_color(context->drawing_context, color);
    cairo_arc(
        context->drawing_context,
        centre.x,
        centre.y,
        radius,
        0.0,
        6.28318530717958647692);
    cairo_fill(context->drawing_context);
    return cairo_result(context);
}

/* Draw one short scene-owned label using the configured adapter font. */
static UmiStatus draw_text(
    void *opaque_context,
    UmiChartRenderPoint position,
    const char *text,
    UmiChartColor color)
{
    UmiChartCairoRendererContext *context =
        (UmiChartCairoRendererContext *)opaque_context;

    /* Text needs an active frame and a non-empty scene-owned string. */
    if (context == NULL || context->drawing_context == NULL ||
        !context->frame_active || text == NULL || text[0] == '\0') {
        return UMI_STATUS_INVALID_STATE;
    }

    set_color(context->drawing_context, color);
    cairo_select_font_face(
        context->drawing_context,
        context->font_family,
        CAIRO_FONT_SLANT_NORMAL,
        CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(context->drawing_context, context->font_size);
    cairo_move_to(context->drawing_context, position.x, position.y);
    cairo_show_text(context->drawing_context, text);
    return cairo_result(context);
}

/* Restore the caller's graphics state after every completed or failed frame. */
static UmiStatus end_frame(void *opaque_context)
{
    UmiChartCairoRendererContext *context =
        (UmiChartCairoRendererContext *)opaque_context;

    /* The save and restore calls must remain balanced exactly once per frame. */
    if (context == NULL || context->drawing_context == NULL ||
        !context->frame_active) {
        return UMI_STATUS_INVALID_STATE;
    }

    cairo_restore(context->drawing_context);
    context->frame_active = 0;
    return cairo_result(context);
}

/* Initialise a Cairo context and return a complete generic renderer descriptor. */
UmiStatus umi_chart_cairo_renderer_init(
    UmiChartCairoRendererContext *context,
    cairo_t *drawing_context,
    UmiChartRenderer *out_renderer)
{
    /* All three records are required because none is owned by this adapter. */
    if (context == NULL || drawing_context == NULL || out_renderer == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *context = (UmiChartCairoRendererContext){0};
    context->drawing_context = drawing_context;
    (void)memcpy(context->font_family, "Sans", sizeof("Sans"));
    context->font_size = 11.0;

    *out_renderer = (UmiChartRenderer){
        .struct_size = (uint32_t)sizeof(*out_renderer),
        .api_version = UMI_CHART_RENDERER_API_VERSION,
        .capabilities = UMI_CHART_RENDERER_CAPABILITY_LINE |
                        UMI_CHART_RENDERER_CAPABILITY_RECTANGLE |
                        UMI_CHART_RENDERER_CAPABILITY_CIRCLE |
                        UMI_CHART_RENDERER_CAPABILITY_TEXT,
        .context = context,
        .operations = {
            .begin = begin_frame,
            .draw_line = draw_line,
            .fill_rectangle = fill_rectangle,
            .stroke_rectangle = stroke_rectangle,
            .fill_circle = fill_circle,
            .draw_text = draw_text,
            .end = end_frame
        }
    };
    /* Reject a drawing context that already carries an unrecoverable backend error. */
    if (cairo_result(context) != UMI_STATUS_OK) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return umi_chart_renderer_validate(out_renderer);
}

/* Change the bounded text family and size used by later scene labels. */
UmiStatus umi_chart_cairo_renderer_set_font(
    UmiChartCairoRendererContext *context,
    const char *font_family,
    double font_size)
{
    size_t length;

    /* Font changes require an idle adapter and a usable name and size. */
    if (context == NULL || font_family == NULL || font_family[0] == '\0' ||
        context->frame_active || !isfinite(font_size) || font_size <= 0.0 ||
        font_size > 512.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(font_family);
    /* Reject truncation so a requested family never becomes a different name. */
    if (length >= sizeof(context->font_family)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(context->font_family, font_family, length + 1U);
    context->font_size = font_size;
    return UMI_STATUS_OK;
}
