/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/geometry.h
 *
 * PURPOSE:
 *   Map canonical UI placement values to deterministic normalized workspace geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_GEOMETRY_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_GEOMETRY_H

#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Normalized rectangle used to place one layout region on any screen size.
 *
 * Values are fractions of the available workspace. For example, a width of
 * `0.25` uses one quarter of the available horizontal space.
 */
typedef struct UmiApplicationSuiteLayoutRect {
    double x;
    double y;
    double width;
    double height;
} UmiApplicationSuiteLayoutRect;

/**
 * Choose which part of a canvas rectangle follows a pointer gesture.
 * Moving keeps the size; south-east resizing keeps the top-left corner.
 */
typedef enum UmiApplicationSuiteLayoutCanvasGesture {
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE = 1,
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST = 2
} UmiApplicationSuiteLayoutCanvasGesture;

/**
 * Check that a canvas rectangle fits entirely inside its normalized viewport.
 *
 * @param rect Borrowed rectangle; coordinates must be finite, sizes positive.
 * @return Non-zero for a rectangle inside [0, 1], or zero for invalid input.
 */
int umi_application_suite_layout_canvas_rect_valid(
    const UmiApplicationSuiteLayoutRect *rect);

/**
 * Convert pointer movement into a bounded canvas rectangle without saving it.
 *
 * All pixels are logical pixels in the same coordinate system as the canvas
 * viewport, not physical monitor pixels. Each update starts from the rectangle
 * captured at gesture start and uses the total pointer delta since that start.
 * This avoids rounding drift when a frontend receives many motion events.
 *
 * Moving preserves size. Resizing keeps x/y fixed and limits each requested
 * minimum to the space remaining beside that corner. A grid below floating
 * point precision is treated as disabled. The caller remains responsible for
 * layout locks, permissions and saving through its workspace transaction.
 *
 * @param start Borrowed valid normalized rectangle at gesture start.
 * @param gesture Move the panel or resize its south-east corner.
 * @param delta_x_pixels Total horizontal pointer movement; negative goes left.
 * @param delta_y_pixels Total vertical pointer movement; negative goes up.
 * @param viewport_width_pixels Positive finite width of the canvas viewport.
 * @param viewport_height_pixels Positive finite height of the canvas viewport.
 * @param min_width_pixels Positive finite minimum panel width when resizing.
 * @param min_height_pixels Positive finite minimum panel height when resizing.
 * @param grid_size Normalized grid spacing in (0, 1], or zero for no snapping.
 * @param out_rect Receives the projected rectangle; may be the same as start.
 * @return UMI_STATUS_OK, or UMI_STATUS_INVALID_ARGUMENT without changing output.
 */
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
    UmiApplicationSuiteLayoutRect *out_rect);

/**
 * Returns the canonical rectangle for one supported placement.
 *
 * @param placement Toolkit-neutral region requested by a layout recipe.
 * @return Normalized geometry. Unsupported placements return the shared
 *         fallback rectangle and can be detected with the support function.
 *
 * @see umi_application_suite_layout_region_supported
 */
UmiApplicationSuiteLayoutRect umi_application_suite_layout_region_rect(
    UmiUiPlacement placement);
/**
 * Reports whether canonical geometry exists for a placement.
 *
 * @param placement Toolkit-neutral region to examine.
 * @return Non-zero when the region can be projected, otherwise zero.
 */
int umi_application_suite_layout_region_supported(UmiUiPlacement placement);

#ifdef __cplusplus
}
#endif
#endif
