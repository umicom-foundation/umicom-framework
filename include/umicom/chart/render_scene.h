/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/render_scene.h
 *
 * PURPOSE:
 *   Store a bounded, toolkit-neutral list of visual chart commands.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

/*
 * A render scene is the hand-off point between chart calculations and a
 * graphics adapter.  Plot builders add ordinary lines, rectangles, circles
 * and labels.  A desktop, web, image or print adapter then draws those same
 * commands without the chart logic knowing which graphics library is in use.
 */

#ifndef UMICOM_CHART_RENDER_SCENE_H
#define UMICOM_CHART_RENDER_SCENE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_RENDER_TEXT_CAPACITY 128U
#define UMI_CHART_RENDER_SCENE_MAX_COMMANDS 65536U

/* Name the primitive operations understood by every chart renderer. */
typedef enum UmiChartRenderCommandKind {
    UMI_CHART_RENDER_LINE = 1,
    UMI_CHART_RENDER_FILL_RECTANGLE = 2,
    UMI_CHART_RENDER_STROKE_RECTANGLE = 3,
    UMI_CHART_RENDER_CIRCLE = 4,
    UMI_CHART_RENDER_TEXT = 5
} UmiChartRenderCommandKind;

/* Store a colour using renderer-independent red, green, blue and alpha values. */
typedef struct UmiChartColor {
    double red;
    double green;
    double blue;
    double alpha;
} UmiChartColor;

/* Store a point in logical pixels relative to the renderer's target. */
typedef struct UmiChartRenderPoint {
    double x;
    double y;
} UmiChartRenderPoint;

/* Store a rectangle whose width and height extend from its top-left corner. */
typedef struct UmiChartRenderRectangle {
    double x;
    double y;
    double width;
    double height;
} UmiChartRenderRectangle;

/*
 * Store one complete command by value.  Text is copied into bounded storage,
 * so a scene never keeps a pointer to temporary caller-owned memory.
 */
typedef struct UmiChartRenderCommand {
    uint32_t struct_size;
    uint32_t api_version;
    UmiChartRenderCommandKind kind;
    UmiChartColor color;
    double stroke_width;
    UmiChartRenderPoint start;
    UmiChartRenderPoint end;
    UmiChartRenderRectangle rectangle;
    double radius;
    char text[UMI_CHART_RENDER_TEXT_CAPACITY];
} UmiChartRenderCommand;

/* Hide scene allocation details so capacity changes do not break callers. */
typedef struct UmiChartRenderScene UmiChartRenderScene;

/* Allocate an empty scene with a fixed maximum number of commands. */
UmiStatus umi_chart_render_scene_create(
    size_t capacity,
    UmiChartRenderScene **out_scene
);

/* Create an independent value copy that can outlive the source scene. */
UmiStatus umi_chart_render_scene_clone(
    const UmiChartRenderScene *source,
    UmiChartRenderScene **out_scene
);

/* Release a scene returned by create or clone. NULL is accepted. */
void umi_chart_render_scene_destroy(UmiChartRenderScene *scene);

/* Remove all commands while retaining the scene's allocated capacity. */
void umi_chart_render_scene_clear(UmiChartRenderScene *scene);

/*
 * Set the logical coordinate size used when the final target is resized.
 * Commands are scaled from this size into the renderer's actual target.
 */
UmiStatus umi_chart_render_scene_set_coordinate_size(
    UmiChartRenderScene *scene,
    double width,
    double height
);

/* Read the logical coordinate size; zero values mean no automatic scaling. */
UmiStatus umi_chart_render_scene_coordinate_size(
    const UmiChartRenderScene *scene,
    double *out_width,
    double *out_height
);

/* Return the number of commands currently stored in the scene. */
size_t umi_chart_render_scene_count(const UmiChartRenderScene *scene);

/* Return the maximum number of commands the scene can store. */
size_t umi_chart_render_scene_capacity(const UmiChartRenderScene *scene);

/* Copy one command out of the scene without exposing internal storage. */
UmiStatus umi_chart_render_scene_at(
    const UmiChartRenderScene *scene,
    size_t index,
    UmiChartRenderCommand *out_command
);

/* Add a straight line such as a grid line, graph segment or candle wick. */
UmiStatus umi_chart_render_scene_add_line(
    UmiChartRenderScene *scene,
    UmiChartRenderPoint start,
    UmiChartRenderPoint end,
    UmiChartColor color,
    double stroke_width
);

/* Add a solid rectangle such as a candle body, bar or background. */
UmiStatus umi_chart_render_scene_add_filled_rectangle(
    UmiChartRenderScene *scene,
    UmiChartRenderRectangle rectangle,
    UmiChartColor color
);

/* Add a rectangle outline such as a plot border or selection. */
UmiStatus umi_chart_render_scene_add_stroked_rectangle(
    UmiChartRenderScene *scene,
    UmiChartRenderRectangle rectangle,
    UmiChartColor color,
    double stroke_width
);

/* Add a filled circular marker used by scatter plots and selected points. */
UmiStatus umi_chart_render_scene_add_circle(
    UmiChartRenderScene *scene,
    UmiChartRenderPoint centre,
    double radius,
    UmiChartColor color
);

/* Add a short copied label at a logical pixel position. */
UmiStatus umi_chart_render_scene_add_text(
    UmiChartRenderScene *scene,
    UmiChartRenderPoint position,
    const char *text,
    UmiChartColor color
);

#ifdef __cplusplus
}
#endif

#endif
