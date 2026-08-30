/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/toolbar.c
 *
 * PURPOSE:
 *   Implement reusable toolbar contribution and state projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/toolbar.h"
#include "internal.h"


void umi_workbench_designer_toolbar_init(UmiWorkbenchDesignerToolbar *toolbar)
{
    if (toolbar == NULL) return;
    (void)memset(toolbar, 0, sizeof(*toolbar));
}

const UmiWorkbenchDesignerToolbarItem *umi_workbench_designer_toolbar_find(
    const UmiWorkbenchDesignerToolbar *toolbar,
    const char *item_id)
{
    size_t index;
    if (toolbar == NULL || item_id == NULL) return NULL;
    for (index = 0U; index < toolbar->count; ++index) {
        if (strcmp(toolbar->items[index].item_id, item_id) == 0) return &toolbar->items[index];
    }
    return NULL;
}

UmiStatus umi_workbench_designer_toolbar_add(
    UmiWorkbenchDesignerToolbar *toolbar,
    const UmiWorkbenchDesignerToolbarItem *item)
{
    if (toolbar == NULL || item == NULL || item->item_id[0] == '\0' ||
        item->command_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_workbench_designer_toolbar_find(toolbar, item->item_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (toolbar->count >= UMI_WORKBENCH_DESIGNER_MAX_TOOLBAR_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    toolbar->items[toolbar->count++] = *item;
    toolbar->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus toolbar_add(
    UmiWorkbenchDesignerToolbar *toolbar,
    const char *id,
    const char *label,
    const char *icon,
    const char *command,
    const char *group,
    uint32_t order,
    bool toggle)
{
    UmiWorkbenchDesignerToolbarItem item;
    (void)memset(&item, 0, sizeof(item));
    (void)umi_workbench_designer_copy_text(item.item_id, sizeof(item.item_id), id);
    (void)umi_workbench_designer_copy_text(item.label, sizeof(item.label), label);
    (void)umi_workbench_designer_copy_text(item.icon_resource_id, sizeof(item.icon_resource_id), icon);
    (void)umi_workbench_designer_copy_text(item.command_id, sizeof(item.command_id), command);
    (void)umi_workbench_designer_copy_text(item.group_id, sizeof(item.group_id), group);
    item.order = order;
    item.enabled = true;
    item.visible = true;
    item.toggle = toggle;
    return umi_workbench_designer_toolbar_add(toolbar, &item);
}

UmiStatus umi_workbench_designer_toolbar_seed(UmiWorkbenchDesignerToolbar *toolbar)
{
    UmiStatus status;
    if (toolbar == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = toolbar_add(toolbar, "select", "Select", "umicom.icon.action.select", "designer.tool.select", "tools", 10U, true);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "pan", "Pan", "umicom.icon.action.pan", "designer.tool.pan", "tools", 20U, true);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "split-h", "Horizontal split", "umicom.icon.action.split-horizontal", "designer.split-horizontal", "structure", 30U, false);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "split-v", "Vertical split", "umicom.icon.action.split-vertical", "designer.split-vertical", "structure", 40U, false);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "tabs", "Tab group", "umicom.icon.action.tabs", "designer.tab-group", "structure", 50U, false);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "float", "Float", "umicom.icon.action.float", "designer.float", "structure", 60U, false);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "grid", "Grid", "umicom.icon.action.grid", "designer.toggle-grid", "view", 70U, true);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "guides", "Guides", "umicom.icon.action.guides", "designer.toggle-guides", "view", 80U, true);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "undo", "Undo", "umicom.icon.action.undo", "designer.undo", "history", 90U, false);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "redo", "Redo", "umicom.icon.action.redo", "designer.redo", "history", 100U, false);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "preview", "Preview", "umicom.icon.action.preview", "designer.preview", "mode", 110U, true);
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "save", "Save", "umicom.icon.action.save", "designer.save", "file", 120U, false);
    return status;
}

UmiStatus umi_workbench_designer_toolbar_set_state(
    UmiWorkbenchDesignerToolbar *toolbar,
    const char *item_id,
    bool enabled,
    bool visible,
    bool checked)
{
    size_t index;
    if (toolbar == NULL || item_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < toolbar->count; ++index) {
        if (strcmp(toolbar->items[index].item_id, item_id) == 0) {
            toolbar->items[index].enabled = enabled;
            toolbar->items[index].visible = visible;
            toolbar->items[index].checked = toolbar->items[index].toggle && checked;
            toolbar->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
