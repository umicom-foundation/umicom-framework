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
    /* Validate before writing output, so callers can keep their last preview
     * when a viewport is not allocated yet or a motion event is malformed. */
    if (out_rect == NULL || !umi_application_suite_layout_canvas_rect_valid(start) ||
        (gesture != UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE &&
         gesture != UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST) ||
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
    /* Dragging changes only the top-left corner, retaining the saved size. */
    if (gesture == UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE) {
        projected.x = canvas_clamp(canvas_snap(start->x + dx, grid_size),
                                    0.0, canvas_available_size(start->width));
        projected.y = canvas_clamp(canvas_snap(start->y + dy, grid_size),
                                    0.0, canvas_available_size(start->height));
    } else {
        double max_width = canvas_available_size(start->x);
        double max_height = canvas_available_size(start->y);
        double min_width = min_width_pixels >= viewport_width_pixels
            ? 1.0 : min_width_pixels / viewport_width_pixels;
        double min_height = min_height_pixels >= viewport_height_pixels
            ? 1.0 : min_height_pixels / viewport_height_pixels;
        /* Very small viewports may not have enough room for the requested
         * minimum. Keep the corner fixed and use the available space. DBL_MIN
         * prevents a positive pixel minimum underflowing to an empty panel. */
        min_width = canvas_clamp(min_width, DBL_MIN, max_width);
        min_height = canvas_clamp(min_height, DBL_MIN, max_height);
        projected.width = canvas_clamp(
            canvas_snap(start->width + dx, grid_size), min_width, max_width);
        projected.height = canvas_clamp(
            canvas_snap(start->height + dy, grid_size), min_height, max_height);
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
