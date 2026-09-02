/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/render_scene.c
 *
 * PURPOSE:
 *   Own bounded chart drawing commands independently of a graphics toolkit.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/render_scene.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Keep the command array private so callers cannot write past its capacity. */
struct UmiChartRenderScene {
    UmiChartRenderCommand *commands;
    size_t count;
    size_t capacity;
    double coordinate_width;
    double coordinate_height;
};

/* Return one only when every colour channel is finite and inside its normal range. */
static int color_is_valid(UmiChartColor color)
{
    return isfinite(color.red) && isfinite(color.green) &&
           isfinite(color.blue) && isfinite(color.alpha) &&
           color.red >= 0.0 && color.red <= 1.0 &&
           color.green >= 0.0 && color.green <= 1.0 &&
           color.blue >= 0.0 && color.blue <= 1.0 &&
           color.alpha >= 0.0 && color.alpha <= 1.0;
}

/* Return one when a logical point contains two usable finite coordinates. */
static int point_is_valid(UmiChartRenderPoint point)
{
    return isfinite(point.x) && isfinite(point.y);
}

/* Return one when a rectangle has finite coordinates and a positive visible size. */
static int rectangle_is_valid(UmiChartRenderRectangle rectangle)
{
    return isfinite(rectangle.x) && isfinite(rectangle.y) &&
           isfinite(rectangle.width) && isfinite(rectangle.height) &&
           rectangle.width > 0.0 && rectangle.height > 0.0;
}

/* Append a fully validated value command without exposing the scene's array. */
static UmiStatus append_command(
    UmiChartRenderScene *scene,
    const UmiChartRenderCommand *command)
{
    /* Both values are required because the scene owns a copy of the command. */
    if (scene == NULL || command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Reject overflow before indexing the fixed-capacity allocation. */
    if (scene->count >= scene->capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    scene->commands[scene->count] = *command;
    ++scene->count;
    return UMI_STATUS_OK;
}

/* Allocate an empty scene with a fixed maximum number of commands. */
UmiStatus umi_chart_render_scene_create(
    size_t capacity,
    UmiChartRenderScene **out_scene)
{
    UmiChartRenderScene *scene;

    /* Clear the output first so a failed call never leaves a stale pointer. */
    if (out_scene == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_scene = NULL;

    /* A bounded positive capacity prevents useless and unexpectedly huge allocations. */
    if (capacity == 0U || capacity > UMI_CHART_RENDER_SCENE_MAX_COMMANDS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    scene = (UmiChartRenderScene *)calloc(1U, sizeof(*scene));
    /* Report allocation failure before dereferencing the result. */
    if (scene == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    scene->commands = (UmiChartRenderCommand *)calloc(
        capacity,
        sizeof(scene->commands[0]));
    /* Release the first allocation if the command store cannot be created. */
    if (scene->commands == NULL) {
        free(scene);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    scene->capacity = capacity;
    *out_scene = scene;
    return UMI_STATUS_OK;
}

/* Create an independent value copy that can outlive the source scene. */
UmiStatus umi_chart_render_scene_clone(
    const UmiChartRenderScene *source,
    UmiChartRenderScene **out_scene)
{
    UmiChartRenderScene *copy;
    UmiStatus status;

    /* A source and output location are both required for a complete clone. */
    if (source == NULL || out_scene == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_chart_render_scene_create(source->capacity, &copy);
    /* Preserve the allocation result so callers learn the actual failure cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Copy only initialised commands; unused capacity remains safely zeroed. */
    if (source->count > 0U) {
        (void)memcpy(copy->commands,
                     source->commands,
                     source->count * sizeof(source->commands[0]));
    }
    copy->count = source->count;
    copy->coordinate_width = source->coordinate_width;
    copy->coordinate_height = source->coordinate_height;
    *out_scene = copy;
    return UMI_STATUS_OK;
}

/* Release a scene returned by create or clone. NULL is accepted. */
void umi_chart_render_scene_destroy(UmiChartRenderScene *scene)
{
    /* Nothing needs releasing when the caller has no scene. */
    if (scene == NULL) {
        return;
    }

    free(scene->commands);
    scene->commands = NULL;
    free(scene);
}

/* Remove all commands while retaining the scene's allocated capacity. */
void umi_chart_render_scene_clear(UmiChartRenderScene *scene)
{
    /* A missing scene is treated as an already-empty scene. */
    if (scene == NULL) {
        return;
    }

    scene->count = 0U;
}

/* Set the logical coordinate size used when the final target is resized. */
UmiStatus umi_chart_render_scene_set_coordinate_size(
    UmiChartRenderScene *scene,
    double width,
    double height)
{
    /* Both dimensions must describe a finite visible coordinate space. */
    if (scene == NULL || !isfinite(width) || !isfinite(height) ||
        width <= 0.0 || height <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    scene->coordinate_width = width;
    scene->coordinate_height = height;
    return UMI_STATUS_OK;
}

/* Read the logical coordinate size; zero values mean no automatic scaling. */
UmiStatus umi_chart_render_scene_coordinate_size(
    const UmiChartRenderScene *scene,
    double *out_width,
    double *out_height)
{
    /* Two output locations are required because width and height form one contract. */
    if (scene == NULL || out_width == NULL || out_height == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_width = scene->coordinate_width;
    *out_height = scene->coordinate_height;
    return UMI_STATUS_OK;
}

/* Return the number of commands currently stored in the scene. */
size_t umi_chart_render_scene_count(const UmiChartRenderScene *scene)
{
    return scene != NULL ? scene->count : 0U;
}

/* Return the maximum number of commands the scene can store. */
size_t umi_chart_render_scene_capacity(const UmiChartRenderScene *scene)
{
    return scene != NULL ? scene->capacity : 0U;
}

/* Copy one command out of the scene without exposing internal storage. */
UmiStatus umi_chart_render_scene_at(
    const UmiChartRenderScene *scene,
    size_t index,
    UmiChartRenderCommand *out_command)
{
    /* The output cannot be populated unless both pointers are valid. */
    if (scene == NULL || out_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Reject missing entries before reading the command array. */
    if (index >= scene->count) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_command = scene->commands[index];
    return UMI_STATUS_OK;
}

/* Add a straight line such as a grid line, graph segment or candle wick. */
UmiStatus umi_chart_render_scene_add_line(
    UmiChartRenderScene *scene,
    UmiChartRenderPoint start,
    UmiChartRenderPoint end,
    UmiChartColor color,
    double stroke_width)
{
    UmiChartRenderCommand command = {0};

    /* Invalid geometry or colour would otherwise reach every backend differently. */
    if (!point_is_valid(start) || !point_is_valid(end) ||
        !color_is_valid(color) || !isfinite(stroke_width) ||
        stroke_width <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    command.struct_size = (uint32_t)sizeof(command);
    command.api_version = 1U;
    command.kind = UMI_CHART_RENDER_LINE;
    command.color = color;
    command.stroke_width = stroke_width;
    command.start = start;
    command.end = end;
    return append_command(scene, &command);
}

/* Add a solid rectangle such as a candle body, bar or background. */
UmiStatus umi_chart_render_scene_add_filled_rectangle(
    UmiChartRenderScene *scene,
    UmiChartRenderRectangle rectangle,
    UmiChartColor color)
{
    UmiChartRenderCommand command = {0};

    /* Reject invisible or unsafe geometry before it reaches a graphics library. */
    if (!rectangle_is_valid(rectangle) || !color_is_valid(color)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    command.struct_size = (uint32_t)sizeof(command);
    command.api_version = 1U;
    command.kind = UMI_CHART_RENDER_FILL_RECTANGLE;
    command.color = color;
    command.rectangle = rectangle;
    return append_command(scene, &command);
}

/* Add a rectangle outline such as a plot border or selection. */
UmiStatus umi_chart_render_scene_add_stroked_rectangle(
    UmiChartRenderScene *scene,
    UmiChartRenderRectangle rectangle,
    UmiChartColor color,
    double stroke_width)
{
    UmiChartRenderCommand command = {0};

    /* A positive finite stroke is required for consistent backend behaviour. */
    if (!rectangle_is_valid(rectangle) || !color_is_valid(color) ||
        !isfinite(stroke_width) || stroke_width <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    command.struct_size = (uint32_t)sizeof(command);
    command.api_version = 1U;
    command.kind = UMI_CHART_RENDER_STROKE_RECTANGLE;
    command.color = color;
    command.stroke_width = stroke_width;
    command.rectangle = rectangle;
    return append_command(scene, &command);
}

/* Add a filled circular marker used by scatter plots and selected points. */
UmiStatus umi_chart_render_scene_add_circle(
    UmiChartRenderScene *scene,
    UmiChartRenderPoint centre,
    double radius,
    UmiChartColor color)
{
    UmiChartRenderCommand command = {0};

    /* A marker needs a finite centre, positive radius and valid colour. */
    if (!point_is_valid(centre) || !isfinite(radius) || radius <= 0.0 ||
        !color_is_valid(color)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    command.struct_size = (uint32_t)sizeof(command);
    command.api_version = 1U;
    command.kind = UMI_CHART_RENDER_CIRCLE;
    command.color = color;
    command.start = centre;
    command.radius = radius;
    return append_command(scene, &command);
}

/* Add a short copied label at a logical pixel position. */
UmiStatus umi_chart_render_scene_add_text(
    UmiChartRenderScene *scene,
    UmiChartRenderPoint position,
    const char *text,
    UmiChartColor color)
{
    UmiChartRenderCommand command = {0};
    size_t length;

    /* Empty labels are not useful and must not be passed to strlen. */
    if (!point_is_valid(position) || text == NULL || text[0] == '\0' ||
        !color_is_valid(color)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(text);
    /* Reject truncation so a displayed value never silently changes meaning. */
    if (length >= sizeof(command.text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    command.struct_size = (uint32_t)sizeof(command);
    command.api_version = 1U;
    command.kind = UMI_CHART_RENDER_TEXT;
    command.color = color;
    command.start = position;
    (void)memcpy(command.text, text, length + 1U);
    return append_command(scene, &command);
}
