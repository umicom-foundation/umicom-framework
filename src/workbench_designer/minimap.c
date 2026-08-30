/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/minimap.c
 *
 * PURPOSE:
 *   Scale canvas geometry into a compact overview and map minimap interaction
 *   back into world coordinates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/minimap.h"
#include "internal.h"
#include <math.h>


void umi_workbench_designer_minimap_init(UmiWorkbenchDesignerMinimap *minimap)
{
    if (minimap == NULL) return;
    (void)memset(minimap, 0, sizeof(*minimap));
    minimap->scale = 1.0;
}

static UmiWorkbenchDesignerRect minimap_project_rect(
    UmiWorkbenchDesignerRect rect,
    UmiWorkbenchDesignerRect content,
    double scale)
{
    UmiWorkbenchDesignerRect result;
    result.x = (rect.x - content.x) * scale;
    result.y = (rect.y - content.y) * scale;
    result.width = rect.width * scale;
    result.height = rect.height * scale;
    return result;
}

UmiStatus umi_workbench_designer_minimap_build(
    UmiWorkbenchDesignerMinimap *minimap,
    const UmiWorkbenchDesignerCanvas *canvas,
    const UmiWorkbenchDesignerViewport *viewport,
    UmiWorkbenchDesignerSize surface_size)
{
    size_t index;
    double scale_x;
    double scale_y;
    UmiWorkbenchDesignerRect visible_world;
    if (minimap == NULL || canvas == NULL || viewport == NULL ||
        surface_size.width <= 0.0 || surface_size.height <= 0.0 ||
        !umi_workbench_designer_rect_is_valid(&canvas->world_bounds)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    minimap->count = 0U;
    minimap->content_bounds = canvas->world_bounds;
    minimap->surface_size = surface_size;
    scale_x = surface_size.width / canvas->world_bounds.width;
    scale_y = surface_size.height / canvas->world_bounds.height;
    minimap->scale = fmin(scale_x, scale_y);
    for (index = 0U; index < canvas->count; ++index) {
        const UmiWorkbenchDesignerCanvasItem *item = &canvas->items[index];
        UmiWorkbenchDesignerMinimapItem *target;
        if (!item->visible || minimap->count >= UMI_WORKBENCH_DESIGNER_MAX_CANVAS_ITEMS) {
            continue;
        }
        target = &minimap->items[minimap->count++];
        (void)memset(target, 0, sizeof(*target));
        (void)umi_workbench_designer_copy_text(
            target->node_id, sizeof(target->node_id), item->node_id);
        target->bounds = minimap_project_rect(
            item->world_bounds, canvas->world_bounds, minimap->scale);
        target->kind = item->kind;
        target->selected = item->selected;
        target->active = item->primary;
    }
    visible_world.x = viewport->origin.x;
    visible_world.y = viewport->origin.y;
    visible_world.width = viewport->canvas_size.width / viewport->zoom;
    visible_world.height = viewport->canvas_size.height / viewport->zoom;
    minimap->viewport_bounds = minimap_project_rect(
        visible_world, canvas->world_bounds, minimap->scale);
    minimap->canvas_revision = canvas->revision;
    minimap->viewport_revision = viewport->revision;
    minimap->revision += 1U;
    return UMI_STATUS_OK;
}

UmiWorkbenchDesignerPoint umi_workbench_designer_minimap_world_point(
    const UmiWorkbenchDesignerMinimap *minimap,
    UmiWorkbenchDesignerPoint minimap_point)
{
    UmiWorkbenchDesignerPoint result = {0.0, 0.0};
    if (minimap == NULL || minimap->scale <= 0.0) return result;
    result.x = minimap->content_bounds.x + minimap_point.x / minimap->scale;
    result.y = minimap->content_bounds.y + minimap_point.y / minimap->scale;
    return result;
}
