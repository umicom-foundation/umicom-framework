/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/toolbar.h
 *
 * PURPOSE:
 *   Describe toolbar groups, toggle state and command availability independently
 *   of GTK widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_TOOLBAR_H
#define UMICOM_WORKBENCH_DESIGNER_TOOLBAR_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer toolbar item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerToolbarItem {
    char item_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char icon_resource_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char command_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint32_t order;
    bool enabled;
    bool visible;
    bool toggle;
    bool checked;
} UmiWorkbenchDesignerToolbarItem;

/**
 * Represent the workbench designer toolbar data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerToolbar {
    UmiWorkbenchDesignerToolbarItem items[UMI_WORKBENCH_DESIGNER_MAX_TOOLBAR_ITEMS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerToolbar;

/**
 * Initialise workbench designer toolbar from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_toolbar_init(UmiWorkbenchDesignerToolbar *toolbar);
/**
 * Provide the workbench designer toolbar seed operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_toolbar_seed(UmiWorkbenchDesignerToolbar *toolbar);
/**
 * Add workbench designer toolbar only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_designer_toolbar_add(UmiWorkbenchDesignerToolbar *toolbar, const UmiWorkbenchDesignerToolbarItem *item);
/**
 * Provide the workbench designer toolbar set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_toolbar_set_state(UmiWorkbenchDesignerToolbar *toolbar, const char *item_id, bool enabled, bool visible, bool checked);
/**
 * Find workbench designer toolbar while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerToolbarItem *umi_workbench_designer_toolbar_find(const UmiWorkbenchDesignerToolbar *toolbar, const char *item_id);

#ifdef __cplusplus
}
#endif

#endif
