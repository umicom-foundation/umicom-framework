/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/toolbar.h
 *
 * PURPOSE:
 *   Describe toolbar groups, toggle state and command availability independently
 *   of GTK widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_TOOLBAR_H
#define UMICOM_WORKBENCH_DESIGNER_TOOLBAR_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


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

typedef struct UmiWorkbenchDesignerToolbar {
    UmiWorkbenchDesignerToolbarItem items[UMI_WORKBENCH_DESIGNER_MAX_TOOLBAR_ITEMS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerToolbar;

void umi_workbench_designer_toolbar_init(UmiWorkbenchDesignerToolbar *toolbar);
UmiStatus umi_workbench_designer_toolbar_seed(UmiWorkbenchDesignerToolbar *toolbar);
UmiStatus umi_workbench_designer_toolbar_add(UmiWorkbenchDesignerToolbar *toolbar, const UmiWorkbenchDesignerToolbarItem *item);
UmiStatus umi_workbench_designer_toolbar_set_state(UmiWorkbenchDesignerToolbar *toolbar, const char *item_id, bool enabled, bool visible, bool checked);
const UmiWorkbenchDesignerToolbarItem *umi_workbench_designer_toolbar_find(const UmiWorkbenchDesignerToolbar *toolbar, const char *item_id);

#ifdef __cplusplus
}
#endif

#endif
