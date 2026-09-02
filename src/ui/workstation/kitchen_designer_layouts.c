/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/kitchen_designer_layouts.c
 *
 * PURPOSE:
 *   Implement Kitchen Designer presets for floor-plan design, 3D review, component catalogues, materials, costing and properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/kitchen_designer_layouts.h"

/* Provide the add operation used by this module and its client applications. */
static UmiStatus add(UmiWsLayoutTemplate *layout, const char *id, UmiWsDockRegion region, int32_t row, int32_t col, double weight) {
    return umi_ws_layout_template_add(layout, id, region, row, col, weight);
}

/*
 * Provide the ws kitchen layout design operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_kitchen_layout_design(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "kitchen.design", "Kitchen Design", UMI_WS_DOMAIN_KITCHEN_DESIGNER);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "kitchen.component-palette", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "kitchen.plan-canvas", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "kitchen.properties", UMI_WS_DOCK_RIGHT, 0, 2, 0.8)) != UMI_STATUS_OK) return s;
    return add(layout, "kitchen.costing", UMI_WS_DOCK_BOTTOM, 1, 1, 0.6);
}

/*
 * Provide the ws kitchen layout review operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_kitchen_layout_review(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "kitchen.review", "3D Review", UMI_WS_DOMAIN_KITCHEN_DESIGNER);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "kitchen.scene-tree", UMI_WS_DOCK_LEFT, 0, 0, 0.7)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "kitchen.3d-view", UMI_WS_DOCK_CENTRE, 0, 1, 2.0)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "kitchen.materials", UMI_WS_DOCK_RIGHT, 0, 2, 0.8)) != UMI_STATUS_OK) return s;
    return add(layout, "kitchen.measurements", UMI_WS_DOCK_BOTTOM, 1, 1, 0.5);
}

/*
 * Provide the ws kitchen layout catalogue operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_kitchen_layout_catalogue(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "kitchen.catalogue", "Catalogue & Materials", UMI_WS_DOMAIN_KITCHEN_DESIGNER);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "kitchen.catalogue-browser", UMI_WS_DOCK_LEFT, 0, 0, 1.0)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "kitchen.asset-grid", UMI_WS_DOCK_CENTRE, 0, 1, 1.6)) != UMI_STATUS_OK) return s;
    return add(layout, "kitchen.asset-inspector", UMI_WS_DOCK_RIGHT, 0, 2, 0.9);
}
