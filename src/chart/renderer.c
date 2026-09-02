/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/renderer.c
 *
 * PURPOSE:
 *   Validate chart graphics adapters and replay toolkit-neutral render scenes.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/renderer.h"

#include <math.h>

/* Return the renderer capability required by one command kind. */
static uint32_t capability_for_command(UmiChartRenderCommandKind kind)
{
    /* Lines require the backend's line operation. */
    if (kind == UMI_CHART_RENDER_LINE) {
        return UMI_CHART_RENDERER_CAPABILITY_LINE;
    }

    /* Filled and outlined rectangles share the rectangle capability. */
    if (kind == UMI_CHART_RENDER_FILL_RECTANGLE ||
        kind == UMI_CHART_RENDER_STROKE_RECTANGLE) {
        return UMI_CHART_RENDERER_CAPABILITY_RECTANGLE;
    }

    /* Circular markers require the backend's circle operation. */
    if (kind == UMI_CHART_RENDER_CIRCLE) {
        return UMI_CHART_RENDERER_CAPABILITY_CIRCLE;
    }

    /* Text labels require the backend's text operation. */
    if (kind == UMI_CHART_RENDER_TEXT) {
        return UMI_CHART_RENDERER_CAPABILITY_TEXT;
    }

    return 0U;
}

/* Check ABI fields, capabilities and required callbacks before rendering. */
UmiStatus umi_chart_renderer_validate(const UmiChartRenderer *renderer)
{
    const uint32_t known_capabilities =
        UMI_CHART_RENDERER_CAPABILITY_LINE |
        UMI_CHART_RENDERER_CAPABILITY_RECTANGLE |
        UMI_CHART_RENDERER_CAPABILITY_CIRCLE |
        UMI_CHART_RENDERER_CAPABILITY_TEXT;

    /* A renderer must carry the complete current contract and lifecycle callbacks. */
    if (renderer == NULL ||
        renderer->struct_size < (uint32_t)sizeof(*renderer) ||
        renderer->api_version != UMI_CHART_RENDERER_API_VERSION ||
        renderer->operations.begin == NULL ||
        renderer->operations.end == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Unknown capability bits indicate a contract version mismatch. */
    if ((renderer->capabilities & ~known_capabilities) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Every advertised capability must have all callbacks needed to fulfil it. */
    if ((renderer->capabilities & UMI_CHART_RENDERER_CAPABILITY_LINE) != 0U &&
        renderer->operations.draw_line == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((renderer->capabilities & UMI_CHART_RENDERER_CAPABILITY_RECTANGLE) != 0U &&
        (renderer->operations.fill_rectangle == NULL ||
         renderer->operations.stroke_rectangle == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((renderer->capabilities & UMI_CHART_RENDERER_CAPABILITY_CIRCLE) != 0U &&
        renderer->operations.fill_circle == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((renderer->capabilities & UMI_CHART_RENDERER_CAPABILITY_TEXT) != 0U &&
        renderer->operations.draw_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/* Dispatch one already-validated scene command to the matching adapter callback. */
static UmiStatus render_command(
    const UmiChartRenderer *renderer,
    const UmiChartRenderCommand *command)
{
    /* A line command maps directly to the line callback. */
    if (command->kind == UMI_CHART_RENDER_LINE) {
        return renderer->operations.draw_line(
            renderer->context,
            command->start,
            command->end,
            command->color,
            command->stroke_width);
    }

    /* A filled rectangle is used for backgrounds, bars and candle bodies. */
    if (command->kind == UMI_CHART_RENDER_FILL_RECTANGLE) {
        return renderer->operations.fill_rectangle(
            renderer->context,
            command->rectangle,
            command->color);
    }

    /* A stroked rectangle is used for borders and selections. */
    if (command->kind == UMI_CHART_RENDER_STROKE_RECTANGLE) {
        return renderer->operations.stroke_rectangle(
            renderer->context,
            command->rectangle,
            command->color,
            command->stroke_width);
    }

    /* A circle is used for scatter and selected-point markers. */
    if (command->kind == UMI_CHART_RENDER_CIRCLE) {
        return renderer->operations.fill_circle(
            renderer->context,
            command->start,
            command->radius,
            command->color);
    }

    /* A text command carries its own bounded, scene-owned label. */
    if (command->kind == UMI_CHART_RENDER_TEXT) {
        return renderer->operations.draw_text(
            renderer->context,
            command->start,
            command->text,
            command->color);
    }

    return UMI_STATUS_NOT_IMPLEMENTED;
}

/* Scale one value command from its scene coordinates into the actual target. */
static UmiStatus scale_command(
    UmiChartRenderCommand *command,
    double scale_x,
    double scale_y)
{
    double measurement_scale = scale_x < scale_y ? scale_x : scale_y;

    /* Every primitive using a point shares the start coordinate fields. */
    command->start.x *= scale_x;
    command->start.y *= scale_y;

    /* Lines use both start and end points. */
    if (command->kind == UMI_CHART_RENDER_LINE) {
        command->end.x *= scale_x;
        command->end.y *= scale_y;
        command->stroke_width *= measurement_scale;
    }

    /* Filled and outlined rectangles scale their origin and complete size. */
    if (command->kind == UMI_CHART_RENDER_FILL_RECTANGLE ||
        command->kind == UMI_CHART_RENDER_STROKE_RECTANGLE) {
        command->rectangle.x *= scale_x;
        command->rectangle.y *= scale_y;
        command->rectangle.width *= scale_x;
        command->rectangle.height *= scale_y;
    }

    /* Only an outlined rectangle uses its stored stroke measurement. */
    if (command->kind == UMI_CHART_RENDER_STROKE_RECTANGLE) {
        command->stroke_width *= measurement_scale;
    }

    /* Circular markers keep their shape by using the smaller target scale. */
    if (command->kind == UMI_CHART_RENDER_CIRCLE) {
        command->radius *= measurement_scale;
    }

    /* Every command carries a start point, including text and circular markers. */
    if (!isfinite(command->start.x) || !isfinite(command->start.y)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* A scaled line must retain finite geometry and a positive stroke. */
    if (command->kind == UMI_CHART_RENDER_LINE &&
        (!isfinite(command->end.x) || !isfinite(command->end.y) ||
         !isfinite(command->stroke_width) || command->stroke_width <= 0.0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* A scaled rectangle must retain finite positive geometry. */
    if ((command->kind == UMI_CHART_RENDER_FILL_RECTANGLE ||
         command->kind == UMI_CHART_RENDER_STROKE_RECTANGLE) &&
        (!isfinite(command->rectangle.x) ||
         !isfinite(command->rectangle.y) ||
         !isfinite(command->rectangle.width) ||
         !isfinite(command->rectangle.height) ||
         command->rectangle.width <= 0.0 ||
         command->rectangle.height <= 0.0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* A scaled rectangle outline must also retain a usable stroke. */
    if (command->kind == UMI_CHART_RENDER_STROKE_RECTANGLE &&
        (!isfinite(command->stroke_width) || command->stroke_width <= 0.0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* A scaled circle must retain a finite positive radius. */
    if (command->kind == UMI_CHART_RENDER_CIRCLE &&
        (!isfinite(command->radius) || command->radius <= 0.0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/* Draw every scene command in order into a target of the requested size. */
UmiStatus umi_chart_renderer_render_scene(
    const UmiChartRenderer *renderer,
    const UmiChartRenderScene *scene,
    double width,
    double height)
{
    UmiChartRenderCommand command;
    UmiStatus status;
    UmiStatus end_status;
    size_t index;
    size_t command_count;
    double coordinate_width;
    double coordinate_height;
    double scale_x = 1.0;
    double scale_y = 1.0;

    /* The scene and finite positive target dimensions are required. */
    if (scene == NULL || !isfinite(width) || !isfinite(height) ||
        width <= 0.0 || height <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_chart_renderer_validate(renderer);
    /* Stop before any callback when the adapter contract is incomplete. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    command_count = umi_chart_render_scene_count(scene);
    status = umi_chart_render_scene_coordinate_size(
        scene,
        &coordinate_width,
        &coordinate_height);
    /* Opaque scene access should succeed; failure indicates invalid internal state. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* A configured coordinate space makes stored scenes responsive to target resizing. */
    if (coordinate_width > 0.0 && coordinate_height > 0.0) {
        scale_x = width / coordinate_width;
        scale_y = height / coordinate_height;
        /* Extreme coordinate sizes must not produce infinite adapter values. */
        if (!isfinite(scale_x) || !isfinite(scale_y) ||
            scale_x <= 0.0 || scale_y <= 0.0) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    /* Preflight every command so unsupported content never leaves a partial drawing. */
    for (index = 0U; index < command_count; ++index) {
        uint32_t required_capability;

        status = umi_chart_render_scene_at(scene, index, &command);
        /* A scene read failure means its internal state is inconsistent. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        /* Validate transformed geometry before begin so drawing remains all-or-nothing. */
        status = scale_command(&command, scale_x, scale_y);
        if (status != UMI_STATUS_OK) {
            return status;
        }

        required_capability = capability_for_command(command.kind);
        /* Reject unknown commands and features the selected backend cannot draw. */
        if (required_capability == 0U ||
            (renderer->capabilities & required_capability) == 0U) {
            return UMI_STATUS_NOT_IMPLEMENTED;
        }
    }

    status = renderer->operations.begin(renderer->context, width, height);
    /* Do not issue drawing operations when the backend could not begin a frame. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Replay commands in their stored order so layers remain deterministic. */
    for (index = 0U; index < command_count; ++index) {
        status = umi_chart_render_scene_at(scene, index, &command);
        /* Stop drawing on the first failed read or backend operation. */
        if (status != UMI_STATUS_OK) {
            break;
        }

        /* Work on the copied command so the reusable source scene remains unchanged. */
        status = scale_command(&command, scale_x, scale_y);
        /* Preflight already checked this path; retain the guard against state corruption. */
        if (status != UMI_STATUS_OK) {
            break;
        }

        status = render_command(renderer, &command);
        /* Preserve the first backend error while still closing the frame below. */
        if (status != UMI_STATUS_OK) {
            break;
        }
    }

    end_status = renderer->operations.end(renderer->context);
    /* Return a drawing failure before a later frame-closing failure. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return end_status;
}
