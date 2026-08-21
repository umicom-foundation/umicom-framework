/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/viewport.c
 *
 * PURPOSE:
 *   Implement stable pan and zoom mathematics independent of GTK adjustment
 *   objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/viewport.h"
#include <math.h>


void umi_workbench_designer_viewport_init(
    UmiWorkbenchDesignerViewport *viewport,
    double width,
    double height)
{
    if (viewport == NULL) return;
    viewport->origin.x = 0.0;
    viewport->origin.y = 0.0;
    viewport->canvas_size.width = width > 0.0 ? width : 1.0;
    viewport->canvas_size.height = height > 0.0 ? height : 1.0;
    viewport->zoom = 1.0;
    viewport->minimum_zoom = 0.10;
    viewport->maximum_zoom = 8.0;
    viewport->revision = 1U;
}

UmiStatus umi_workbench_designer_viewport_set_canvas_size(
    UmiWorkbenchDesignerViewport *viewport,
    double width,
    double height)
{
    if (viewport == NULL || !isfinite(width) || !isfinite(height) ||
        width <= 0.0 || height <= 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    viewport->canvas_size.width = width;
    viewport->canvas_size.height = height;
    viewport->revision += 1U;
    return UMI_STATUS_OK;
}

UmiWorkbenchDesignerPoint umi_workbench_designer_viewport_screen_to_world(
    const UmiWorkbenchDesignerViewport *viewport,
    UmiWorkbenchDesignerPoint point)
{
    UmiWorkbenchDesignerPoint result = {0.0, 0.0};
    if (viewport == NULL || viewport->zoom <= 0.0) return result;
    result.x = viewport->origin.x + point.x / viewport->zoom;
    result.y = viewport->origin.y + point.y / viewport->zoom;
    return result;
}

UmiWorkbenchDesignerPoint umi_workbench_designer_viewport_world_to_screen(
    const UmiWorkbenchDesignerViewport *viewport,
    UmiWorkbenchDesignerPoint point)
{
    UmiWorkbenchDesignerPoint result = {0.0, 0.0};
    if (viewport == NULL) return result;
    result.x = (point.x - viewport->origin.x) * viewport->zoom;
    result.y = (point.y - viewport->origin.y) * viewport->zoom;
    return result;
}

UmiStatus umi_workbench_designer_viewport_set_zoom(
    UmiWorkbenchDesignerViewport *viewport,
    double zoom,
    UmiWorkbenchDesignerPoint anchor_screen)
{
    UmiWorkbenchDesignerPoint before;
    UmiWorkbenchDesignerPoint after;
    double clamped;
    if (viewport == NULL || !isfinite(zoom)) return UMI_STATUS_INVALID_ARGUMENT;
    before = umi_workbench_designer_viewport_screen_to_world(viewport, anchor_screen);
    clamped = umi_workbench_designer_clamp(
        zoom, viewport->minimum_zoom, viewport->maximum_zoom);
    viewport->zoom = clamped;
    after = umi_workbench_designer_viewport_screen_to_world(viewport, anchor_screen);
    viewport->origin.x += before.x - after.x;
    viewport->origin.y += before.y - after.y;
    viewport->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_viewport_pan(
    UmiWorkbenchDesignerViewport *viewport,
    double delta_x,
    double delta_y)
{
    if (viewport == NULL || !isfinite(delta_x) || !isfinite(delta_y) ||
        viewport->zoom <= 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    viewport->origin.x -= delta_x / viewport->zoom;
    viewport->origin.y -= delta_y / viewport->zoom;
    viewport->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_viewport_fit_rect(
    UmiWorkbenchDesignerViewport *viewport,
    const UmiWorkbenchDesignerRect *world_rect,
    double padding)
{
    double usable_width;
    double usable_height;
    double zoom_x;
    double zoom_y;
    if (viewport == NULL || !umi_workbench_designer_rect_is_valid(world_rect) ||
        world_rect->width <= 0.0 || world_rect->height <= 0.0 ||
        !isfinite(padding) || padding < 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    usable_width = viewport->canvas_size.width - padding * 2.0;
    usable_height = viewport->canvas_size.height - padding * 2.0;
    if (usable_width <= 0.0 || usable_height <= 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    zoom_x = usable_width / world_rect->width;
    zoom_y = usable_height / world_rect->height;
    viewport->zoom = umi_workbench_designer_clamp(
        zoom_x < zoom_y ? zoom_x : zoom_y,
        viewport->minimum_zoom,
        viewport->maximum_zoom);
    viewport->origin.x = world_rect->x - padding / viewport->zoom;
    viewport->origin.y = world_rect->y - padding / viewport->zoom;
    viewport->revision += 1U;
    return UMI_STATUS_OK;
}

UmiWorkbenchDesignerRect umi_workbench_designer_viewport_visible_world(
    const UmiWorkbenchDesignerViewport *viewport)
{
    UmiWorkbenchDesignerRect result = {0.0, 0.0, 0.0, 0.0};
    if (viewport == NULL || viewport->zoom <= 0.0) return result;
    result.x = viewport->origin.x;
    result.y = viewport->origin.y;
    result.width = viewport->canvas_size.width / viewport->zoom;
    result.height = viewport->canvas_size.height / viewport->zoom;
    return result;
}
