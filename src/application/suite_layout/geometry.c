/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/geometry.c
 *
 * PURPOSE:
 *   Provide stable single-monitor geometry used to materialise all suite application layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/geometry.h"
#include "umicom/ui/workspace_geometry.h"

#include <float.h>
#include <math.h>

/* Reject invalid saved or projected coordinates before native widget layout. */
int umi_application_suite_layout_canvas_rect_valid(
    const UmiApplicationSuiteLayoutRect *rect)
{
    /* Subtract the size from one instead of adding coordinates, avoiding an
     * overflowing sum when a caller supplies an invalid, very large value. */
    return rect != NULL && isfinite(rect->x) && isfinite(rect->y) &&
        isfinite(rect->width) && isfinite(rect->height) &&
        rect->x >= 0.0 && rect->x < 1.0 &&
        rect->y >= 0.0 && rect->y < 1.0 &&
        rect->width > 0.0 && rect->width <= 1.0 &&
        rect->height > 0.0 && rect->height <= 1.0 &&
        rect->x <= 1.0 - rect->width &&
        rect->y <= 1.0 - rect->height;
}

/* Keep a finite coordinate within the allowed interval. */
static double canvas_clamp(double value, double minimum, double maximum)
{
    /* Bounds are applied after snapping too, so no panel can leave the host. */
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

/* Convert motion to a useful fraction without overflowing on extreme input. */
static double canvas_delta(double pixels, double viewport)
{
    /* A movement larger than the complete viewport already reaches an edge.
     * Limit it before division, including when the viewport is very small. */
    if (pixels >= viewport) return 1.0;
    if (pixels <= -viewport) return -1.0;
    return pixels / viewport;
}

/* Round a normalized value to a nearby grid line without a math-library call. */
static double canvas_snap(double value, double grid)
{
    double bounded = canvas_clamp(value, 0.0, 1.0);
    uint64_t steps;
    /* Sub-precision grids do not visibly move a panel. Skipping them also
     * bounds the quotient before conversion to an unsigned integer. */
    if (grid < DBL_EPSILON) return bounded;
    steps = (uint64_t)(bounded / grid + 0.5);
    return (double)steps * grid;
}

/* Find the remaining size using the same boundary rule as saved layouts. */
static double canvas_available_size(double origin)
{
    double remaining = 1.0 - origin;
    /* Decimal fractions can round differently after the second subtraction.
     * A tiny inward adjustment keeps the resulting size valid for the core
     * workspace check origin <= 1 - size, including panels touching an edge. */
    if (origin > 1.0 - remaining)
        remaining *= 1.0 - DBL_EPSILON;
    return remaining;
}

/* Resize one moving axis from its original opposite anchor. The named low
 * edge is west/north; high edges keep the original x/y coordinate instead. */
static void canvas_resize_axis(
    double origin, double size, double delta, int low_edge,
    double minimum_pixels, double viewport_pixels, double grid,
    double *out_origin, double *out_size)
{
    double anchor = low_edge ? canvas_clamp(origin + size, 0.0, 1.0) : origin;
    double maximum = low_edge ? anchor : canvas_available_size(origin);
    double minimum = minimum_pixels >= viewport_pixels
        ? 1.0 : minimum_pixels / viewport_pixels;
    /* A very small viewport can make the requested minimum impossible. The
     * available space wins; retain positive subnormal space when even DBL_MIN
     * would extend beyond a north/west gesture's fixed opposite edge. */
    double positive_floor = maximum < DBL_MIN ? maximum : DBL_MIN;
    minimum = canvas_clamp(minimum, positive_floor, maximum);
    *out_size = canvas_clamp(canvas_snap(size + (low_edge ? -delta : delta), grid),
        minimum, maximum);
    /* Subtraction preserves the opposite edge, except a sub-precision size
     * at coordinate one needs the established tiny inward boundary limit. */
    *out_origin = low_edge
        ? canvas_clamp(anchor - *out_size, 0.0, canvas_available_size(*out_size))
        : origin;
}

/* Project a gesture into portable geometry; saving remains the host's job. */
UmiStatus umi_application_suite_layout_project_canvas_gesture(
    const UmiApplicationSuiteLayoutRect *start,
    UmiApplicationSuiteLayoutCanvasGesture gesture,
    double delta_x_pixels,
    double delta_y_pixels,
    double viewport_width_pixels,
    double viewport_height_pixels,
    double min_width_pixels,
    double min_height_pixels,
    double grid_size,
    UmiApplicationSuiteLayoutRect *out_rect)
{
    UmiApplicationSuiteLayoutRect projected;
    double dx;
    double dy;
    int horizontal_edge = 0;
    int vertical_edge = 0;
    /* Validate before writing output, so callers can keep their last preview
     * when a viewport is not allocated yet or a motion event is malformed. */
    if (out_rect == NULL || !umi_application_suite_layout_canvas_rect_valid(start) ||
        (gesture < UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE ||
         gesture > UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST) ||
        !isfinite(delta_x_pixels) || !isfinite(delta_y_pixels) ||
        !isfinite(viewport_width_pixels) || viewport_width_pixels <= 0.0 ||
        !isfinite(viewport_height_pixels) || viewport_height_pixels <= 0.0 ||
        !isfinite(min_width_pixels) || min_width_pixels <= 0.0 ||
        !isfinite(min_height_pixels) || min_height_pixels <= 0.0 ||
        !isfinite(grid_size) || grid_size < 0.0 || grid_size > 1.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    projected = *start;
    dx = canvas_delta(delta_x_pixels, viewport_width_pixels);
    dy = canvas_delta(delta_y_pixels, viewport_height_pixels);
    /* Signed edge directions share one axis rule instead of maintaining eight
     * separate geometry implementations. Zero means that axis must not move. */
    switch (gesture) {
        case UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST: horizontal_edge = 1; vertical_edge = 1; break;
        case UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH: vertical_edge = -1; break;
        case UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_EAST: horizontal_edge = 1; vertical_edge = -1; break;
        case UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_EAST: horizontal_edge = 1; break;
        case UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH: vertical_edge = 1; break;
        case UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_WEST: horizontal_edge = -1; vertical_edge = 1; break;
        case UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_WEST: horizontal_edge = -1; break;
        case UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST: horizontal_edge = -1; vertical_edge = -1; break;
        default: break;
    }
    /* Dragging changes only the top-left corner, retaining the saved size. */
    if (gesture == UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE) {
        if (dx != 0.0)
            projected.x = canvas_clamp(canvas_snap(start->x + dx, grid_size),
                                        0.0, canvas_available_size(start->width));
        if (dy != 0.0)
            projected.y = canvas_clamp(canvas_snap(start->y + dy, grid_size),
                                        0.0, canvas_available_size(start->height));
    } else {
        /* Pointer-down and motion along an unrelated axis must not snap or
         * enlarge an imported panel before that edge has actually moved. */
        if (horizontal_edge != 0 && dx != 0.0)
            canvas_resize_axis(start->x, start->width, dx, horizontal_edge < 0,
                min_width_pixels, viewport_width_pixels, grid_size,
                &projected.x, &projected.width);
        if (vertical_edge != 0 && dy != 0.0)
            canvas_resize_axis(start->y, start->height, dy, vertical_edge < 0,
                min_height_pixels, viewport_height_pixels, grid_size,
                &projected.y, &projected.height);
    }
    *out_rect = projected;
    return UMI_STATUS_OK;
}

/*
 * Provide the application suite layout region supported operation used by this module and
 * its client applications.
 */
int umi_application_suite_layout_region_supported(UmiUiPlacement placement)
{
    /* Keep the established application API as a compatibility wrapper around
     * the lower-level UI geometry service. */
    return umi_ui_workspace_region_supported(placement) ? 1 : 0;
}

/* Return shared normalized geometry through the established suite API. */
UmiApplicationSuiteLayoutRect umi_application_suite_layout_region_rect(
    UmiUiPlacement placement)
{
    /* Convert the shared UI rectangle by value so the application layer does
     * not expose or own the lower-level geometry structure. */
    UmiUiWorkspaceRect shared = umi_ui_workspace_region_rect(placement);
    UmiApplicationSuiteLayoutRect rect = {
        shared.x,
        shared.y,
        shared.width,
        shared.height
    };
    return rect;
}
