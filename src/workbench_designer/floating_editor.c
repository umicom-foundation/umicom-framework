/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/floating_editor.c
 *
 * PURPOSE:
 *   Implement floating and docked placement as semantic state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/floating_editor.h"
#include "internal.h"


UmiStatus umi_workbench_designer_float_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    const char *monitor_id,
    UmiWorkbenchDesignerRect bounds,
    int32_t z_order)
{
    UmiWorkbenchLayoutNode *node;
    UmiStatus status;
    UmiWorkbenchLayoutRect layout_bounds;
    if (document == NULL || node_id == NULL ||
        !umi_workbench_designer_rect_is_valid(&bounds) ||
        bounds.width <= 0.0 || bounds.height <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node = umi_workbench_layout_document_find_node_mutable(document, node_id);
    if (node == NULL) return UMI_STATUS_NOT_FOUND;
    layout_bounds = umi_workbench_designer_to_layout_rect(bounds);
    status = umi_workbench_layout_node_set_bounds(node, &layout_bounds);
    if (status == UMI_STATUS_OK && monitor_id != NULL) {
        status = umi_workbench_layout_node_set_monitor(node, monitor_id);
    }
    if (status == UMI_STATUS_OK) {
        node->dock_region = UMI_WORKBENCH_LAYOUT_DOCK_FLOATING;
        node->z_order = z_order;
        node->revision += 1U;
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
    }
    return status;
}

UmiStatus umi_workbench_designer_dock_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    UmiWorkbenchLayoutDockRegion dock_region,
    const char *parent_node_id,
    size_t position)
{
    UmiWorkbenchLayoutNode *node;
    UmiStatus status = UMI_STATUS_OK;
    if (document == NULL || node_id == NULL ||
        dock_region == UMI_WORKBENCH_LAYOUT_DOCK_FLOATING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node = umi_workbench_layout_document_find_node_mutable(document, node_id);
    if (node == NULL) return UMI_STATUS_NOT_FOUND;
    if (parent_node_id != NULL && parent_node_id[0] != '\0') {
        status = umi_workbench_layout_document_move_node(
            document, node_id, parent_node_id, position);
    }
    if (status == UMI_STATUS_OK) {
        node = umi_workbench_layout_document_find_node_mutable(document, node_id);
        node->dock_region = dock_region;
        node->monitor_id[0] = '\0';
        node->z_order = 0;
        node->revision += 1U;
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
    }
    return status;
}
