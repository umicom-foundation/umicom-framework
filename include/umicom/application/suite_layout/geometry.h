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
 * Moving keeps the size. Resizing moves only the named edge or corner while
 * keeping the opposite edge or corner fixed. Existing numeric values stay
 * unchanged for adapters which already use south-east resizing.
 */
typedef enum UmiApplicationSuiteLayoutCanvasGesture {
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE = 1,
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST = 2,
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH = 3,
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_EAST = 4,
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_EAST = 5,
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH = 6,
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_WEST = 7,
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_WEST = 8,
    UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST = 9
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
 * Moving preserves size and snaps the moving origin coordinates. Resizing
 * snaps the affected dimensions, preserving the existing south-east behavior,
 * and limits each requested minimum to the space beside its opposite anchor.
 * Bounds and minimum sizes take precedence over the grid. An untouched axis
 * remains exactly unchanged, even for an off-grid or undersized saved panel;
 * a zero-delta gesture never jumps. Tiny inward rounding may be necessary at
 * a viewport boundary. A grid below floating point precision is disabled.
 * The caller remains responsible for layout locks, permissions and saving
 * through its workspace transaction; projection grants no edit permission.
 *
 * @param start Borrowed valid normalized rectangle at gesture start.
 * @param gesture Move the panel or resize one of its eight edges/corners.
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
