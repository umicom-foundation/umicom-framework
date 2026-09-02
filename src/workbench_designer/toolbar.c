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


/*
 * Initialise workbench designer toolbar from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_toolbar_init(UmiWorkbenchDesignerToolbar *toolbar)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (toolbar == NULL) return;
    (void)memset(toolbar, 0, sizeof(*toolbar));
}

/*
 * Find workbench designer toolbar while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerToolbarItem *umi_workbench_designer_toolbar_find(
    const UmiWorkbenchDesignerToolbar *toolbar,
    const char *item_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (toolbar == NULL || item_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < toolbar->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(toolbar->items[index].item_id, item_id) == 0) return &toolbar->items[index];
    }
    return NULL;
}

/*
 * Add workbench designer toolbar only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_designer_toolbar_add(
    UmiWorkbenchDesignerToolbar *toolbar,
    const UmiWorkbenchDesignerToolbarItem *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (toolbar == NULL || item == NULL || item->item_id[0] == '\0' ||
        item->command_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_workbench_designer_toolbar_find(toolbar, item->item_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (toolbar->count >= UMI_WORKBENCH_DESIGNER_MAX_TOOLBAR_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    toolbar->items[toolbar->count++] = *item;
    toolbar->revision += 1U;
    return UMI_STATUS_OK;
}

/* Add toolbar only after its inputs and available capacity have been checked. */
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

/*
 * Provide the workbench designer toolbar seed operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_toolbar_seed(UmiWorkbenchDesignerToolbar *toolbar)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (toolbar == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = toolbar_add(toolbar, "select", "Select", "umicom.icon.action.select", "designer.tool.select", "tools", 10U, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "pan", "Pan", "umicom.icon.action.pan", "designer.tool.pan", "tools", 20U, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "split-h", "Horizontal split", "umicom.icon.action.split-horizontal", "designer.split-horizontal", "structure", 30U, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "split-v", "Vertical split", "umicom.icon.action.split-vertical", "designer.split-vertical", "structure", 40U, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "tabs", "Tab group", "umicom.icon.action.tabs", "designer.tab-group", "structure", 50U, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "float", "Float", "umicom.icon.action.float", "designer.float", "structure", 60U, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "grid", "Grid", "umicom.icon.action.grid", "designer.toggle-grid", "view", 70U, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "guides", "Guides", "umicom.icon.action.guides", "designer.toggle-guides", "view", 80U, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "undo", "Undo", "umicom.icon.action.undo", "designer.undo", "history", 90U, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "redo", "Redo", "umicom.icon.action.redo", "designer.redo", "history", 100U, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "preview", "Preview", "umicom.icon.action.preview", "designer.preview", "mode", 110U, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = toolbar_add(toolbar, "save", "Save", "umicom.icon.action.save", "designer.save", "file", 120U, false);
    return status;
}

/*
 * Provide the workbench designer toolbar set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_toolbar_set_state(
    UmiWorkbenchDesignerToolbar *toolbar,
    const char *item_id,
    bool enabled,
    bool visible,
    bool checked)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (toolbar == NULL || item_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < toolbar->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
