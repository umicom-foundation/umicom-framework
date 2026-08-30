/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/panel_contribution.c
 *
 * PURPOSE:
 *   Register durable panel identities and default placements for application
 *   composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/panel_contribution.h"
#include "internal.h"


void umi_workbench_designer_panel_catalogue_init(
    UmiWorkbenchDesignerPanelCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
}

const UmiWorkbenchDesignerPanelContribution *umi_workbench_designer_panel_catalogue_find(
    const UmiWorkbenchDesignerPanelCatalogue *catalogue,
    const char *panel_id)
{
    size_t index;
    if (catalogue == NULL || panel_id == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->panels[index].panel_id, panel_id) == 0) {
            return &catalogue->panels[index];
        }
    }
    return NULL;
}

UmiStatus umi_workbench_designer_panel_catalogue_add(
    UmiWorkbenchDesignerPanelCatalogue *catalogue,
    const UmiWorkbenchDesignerPanelContribution *panel)
{
    if (catalogue == NULL || panel == NULL || panel->panel_id[0] == '\0' ||
        panel->title[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_workbench_designer_panel_catalogue_find(catalogue, panel->panel_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (catalogue->count >= 32U) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->panels[catalogue->count++] = *panel;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus panel_add(
    UmiWorkbenchDesignerPanelCatalogue *catalogue,
    const char *id,
    const char *title,
    const char *icon,
    const char *category,
    UmiWorkbenchLayoutDockRegion dock,
    uint32_t order,
    bool default_visible,
    bool document,
    bool selection)
{
    UmiWorkbenchDesignerPanelContribution panel;
    (void)memset(&panel, 0, sizeof(panel));
    (void)umi_workbench_designer_copy_text(panel.panel_id, sizeof(panel.panel_id), id);
    (void)umi_workbench_designer_copy_text(panel.title, sizeof(panel.title), title);
    (void)umi_workbench_designer_copy_text(panel.icon_resource_id, sizeof(panel.icon_resource_id), icon);
    (void)umi_workbench_designer_copy_text(panel.category, sizeof(panel.category), category);
    panel.default_dock = dock;
    panel.order = order;
    panel.singleton = true;
    panel.visible_by_default = default_visible;
    panel.requires_document = document;
    panel.requires_selection = selection;
    return umi_workbench_designer_panel_catalogue_add(catalogue, &panel);
}

UmiStatus umi_workbench_designer_panel_catalogue_seed(
    UmiWorkbenchDesignerPanelCatalogue *catalogue)
{
    UmiStatus status;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = panel_add(catalogue, "umicom.layout.browser", "Layout Browser", "umicom.icon.action.layouts", "layout", UMI_WORKBENCH_LAYOUT_DOCK_LEFT, 10U, true, false, false);
    if (status == UMI_STATUS_OK) status = panel_add(catalogue, "umicom.layout.canvas", "Layout Canvas", "umicom.icon.action.design", "layout", UMI_WORKBENCH_LAYOUT_DOCK_DOCUMENT, 20U, true, true, false);
    if (status == UMI_STATUS_OK) status = panel_add(catalogue, "umicom.layout.palette", "Component Palette", "umicom.icon.action.add", "layout", UMI_WORKBENCH_LAYOUT_DOCK_LEFT, 30U, true, true, false);
    if (status == UMI_STATUS_OK) status = panel_add(catalogue, "umicom.layout.tree", "Layout Tree", "umicom.icon.action.tree", "layout", UMI_WORKBENCH_LAYOUT_DOCK_LEFT, 40U, true, true, false);
    if (status == UMI_STATUS_OK) status = panel_add(catalogue, "umicom.layout.properties", "Properties", "umicom.icon.action.properties", "layout", UMI_WORKBENCH_LAYOUT_DOCK_RIGHT, 50U, true, true, true);
    if (status == UMI_STATUS_OK) status = panel_add(catalogue, "umicom.layout.history", "History", "umicom.icon.action.history", "layout", UMI_WORKBENCH_LAYOUT_DOCK_RIGHT, 60U, false, true, false);
    if (status == UMI_STATUS_OK) status = panel_add(catalogue, "umicom.layout.diff", "Layout Compare", "umicom.icon.action.compare", "layout", UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM, 70U, false, true, false);
    if (status == UMI_STATUS_OK) status = panel_add(catalogue, "umicom.layout.collaboration", "Collaboration", "umicom.icon.action.collaboration", "layout", UMI_WORKBENCH_LAYOUT_DOCK_RIGHT, 80U, false, true, false);
    if (status == UMI_STATUS_OK) status = panel_add(catalogue, "umicom.layout.diagnostics", "Layout Problems", "umicom.icon.status.warning", "layout", UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM, 90U, true, true, false);
    return status;
}
