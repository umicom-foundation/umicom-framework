/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/browser_preview.c
 *
 * PURPOSE:
 *   Generate a lightweight semantic thumbnail and structural counts for Layout
 *   Browser selection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/browser_preview.h"
#include "internal.h"


void umi_workbench_designer_browser_preview_init(
    UmiWorkbenchDesignerBrowserPreview *preview)
{
    if (preview == NULL) return;
    (void)memset(preview, 0, sizeof(*preview));
    umi_workbench_designer_minimap_init(&preview->minimap);
}

UmiStatus umi_workbench_designer_browser_preview_build(
    UmiWorkbenchDesignerBrowserPreview *preview,
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchDesignerSize surface_size)
{
    UmiWorkbenchDesignerCanvas canvas;
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerRect root_bounds = {0.0, 0.0, 1280.0, 800.0};
    size_t index;
    UmiStatus status;
    if (preview == NULL || document == NULL ||
        surface_size.width <= 0.0 || surface_size.height <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_browser_preview_init(preview);
    (void)umi_workbench_designer_copy_text(
        preview->layout_id, sizeof(preview->layout_id),
        document->identity.layout_id);
    (void)umi_workbench_designer_copy_text(
        preview->name, sizeof(preview->name), document->name);
    (void)umi_workbench_designer_copy_text(
        preview->category, sizeof(preview->category), document->category);
    for (index = 0U; index < document->node_count; ++index) {
        const UmiWorkbenchLayoutNode *node = &document->nodes[index];
        if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
            node->kind == UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP) {
            preview->panel_count += 1U;
        }
        if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW) {
            preview->floating_window_count += 1U;
        }
        if (node->monitor_id[0] != '\0') preview->monitor_count += 1U;
    }
    umi_workbench_designer_canvas_init(&canvas);
    umi_workbench_designer_viewport_init(
        &viewport, surface_size.width, surface_size.height);
    umi_workbench_designer_selection_init(&selection);
    status = umi_workbench_designer_canvas_build(
        &canvas, document, &viewport, &selection, root_bounds);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_designer_minimap_build(
        &preview->minimap, &canvas, &viewport, surface_size);
    if (status != UMI_STATUS_OK) return status;
    preview->available = true;
    preview->layout_revision = document->version.revision;
    preview->revision += 1U;
    return UMI_STATUS_OK;
}
