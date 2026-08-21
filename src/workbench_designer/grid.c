/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/grid.c
 *
 * PURPOSE:
 *   Implement bounded grid enumeration without allocating per-frame objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/grid.h"
#include <math.h>


void umi_workbench_designer_grid_init(UmiWorkbenchDesignerGrid *grid)
{
    if (grid == NULL) return;
    grid->enabled = true;
    grid->visible = true;
    grid->spacing = 8.0;
    grid->major_every = 8U;
    grid->origin.x = 0.0;
    grid->origin.y = 0.0;
    grid->revision = 1U;
}

UmiStatus umi_workbench_designer_grid_configure(
    UmiWorkbenchDesignerGrid *grid,
    double spacing,
    uint32_t major_every,
    bool visible,
    bool enabled)
{
    if (grid == NULL || !isfinite(spacing) || spacing <= 0.0 ||
        major_every == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    grid->spacing = spacing;
    grid->major_every = major_every;
    grid->visible = visible;
    grid->enabled = enabled;
    grid->revision += 1U;
    return UMI_STATUS_OK;
}

double umi_workbench_designer_grid_snap_value(
    const UmiWorkbenchDesignerGrid *grid,
    double value,
    double origin)
{
    double relative;
    if (grid == NULL || !grid->enabled || grid->spacing <= 0.0) return value;
    relative = (value - origin) / grid->spacing;
    return origin + floor(relative + 0.5) * grid->spacing;
}

UmiWorkbenchDesignerPoint umi_workbench_designer_grid_snap_point(
    const UmiWorkbenchDesignerGrid *grid,
    UmiWorkbenchDesignerPoint point)
{
    if (grid == NULL) return point;
    point.x = umi_workbench_designer_grid_snap_value(grid, point.x, grid->origin.x);
    point.y = umi_workbench_designer_grid_snap_value(grid, point.y, grid->origin.y);
    return point;
}

static size_t grid_axis_line_count(double start, double length, double origin, double spacing)
{
    const double first = ceil((start - origin) / spacing);
    const double last = floor((start + length - origin) / spacing);
    if (last < first) return 0U;
    return (size_t)(last - first + 1.0);
}

size_t umi_workbench_designer_grid_line_count(
    const UmiWorkbenchDesignerGrid *grid,
    const UmiWorkbenchDesignerRect *visible_world)
{
    if (grid == NULL || !grid->visible || grid->spacing <= 0.0 ||
        !umi_workbench_designer_rect_is_valid(visible_world)) return 0U;
    return grid_axis_line_count(
               visible_world->x, visible_world->width,
               grid->origin.x, grid->spacing) +
           grid_axis_line_count(
               visible_world->y, visible_world->height,
               grid->origin.y, grid->spacing);
}

UmiStatus umi_workbench_designer_grid_line_at(
    const UmiWorkbenchDesignerGrid *grid,
    const UmiWorkbenchDesignerRect *visible_world,
    size_t index,
    UmiWorkbenchDesignerGridLine *out_line)
{
    size_t vertical_count;
    double first;
    double logical_index;
    int64_t integer_index;
    if (grid == NULL || visible_world == NULL || out_line == NULL ||
        !grid->visible || grid->spacing <= 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    vertical_count = grid_axis_line_count(
        visible_world->x, visible_world->width,
        grid->origin.x, grid->spacing);
    if (index >= umi_workbench_designer_grid_line_count(grid, visible_world)) {
        return UMI_STATUS_NOT_FOUND;
    }
    out_line->vertical = index < vertical_count;
    if (out_line->vertical) {
        first = ceil((visible_world->x - grid->origin.x) / grid->spacing);
        logical_index = first + (double)index;
        out_line->coordinate = grid->origin.x + logical_index * grid->spacing;
    } else {
        first = ceil((visible_world->y - grid->origin.y) / grid->spacing);
        logical_index = first + (double)(index - vertical_count);
        out_line->coordinate = grid->origin.y + logical_index * grid->spacing;
    }
    integer_index = (int64_t)logical_index;
    out_line->major = (integer_index % (int64_t)grid->major_every) == 0;
    return UMI_STATUS_OK;
}
