/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_geometry.c
 *
 * PURPOSE:
 *   Implement portable geometry transforms for DPI scaling, monitor clamping and saved workstation layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/layout_geometry.h"

UmiUiRect umi_ws_layout_geometry_clamp(UmiUiRect bounds, UmiUiRect work_area) {
    UmiUiRect result = bounds;
    if (result.width > work_area.width) result.width = work_area.width;
    if (result.height > work_area.height) result.height = work_area.height;
    if (result.width < 1) result.width = 1;
    if (result.height < 1) result.height = 1;
    if (result.x < work_area.x) result.x = work_area.x;
    if (result.y < work_area.y) result.y = work_area.y;
    if (result.x + result.width > work_area.x + work_area.width) result.x = work_area.x + work_area.width - result.width;
    if (result.y + result.height > work_area.y + work_area.height) result.y = work_area.y + work_area.height - result.height;
    return result;
}

UmiUiRect umi_ws_layout_geometry_scale(UmiUiRect bounds, int32_t from_scale_percent, int32_t to_scale_percent) {
    UmiUiRect result = bounds;
    if (from_scale_percent <= 0 || to_scale_percent <= 0) return result;
    result.x = (int32_t)(((int64_t)bounds.x * to_scale_percent) / from_scale_percent);
    result.y = (int32_t)(((int64_t)bounds.y * to_scale_percent) / from_scale_percent);
    result.width = (int32_t)(((int64_t)bounds.width * to_scale_percent) / from_scale_percent);
    result.height = (int32_t)(((int64_t)bounds.height * to_scale_percent) / from_scale_percent);
    return result;
}

int32_t umi_ws_layout_geometry_area(UmiUiRect bounds) {
    int64_t area;
    if (bounds.width <= 0 || bounds.height <= 0) return 0;
    area = (int64_t)bounds.width * (int64_t)bounds.height;
    if (area > INT32_MAX) return INT32_MAX;
    return (int32_t)area;
}
