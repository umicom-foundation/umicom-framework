/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/canvas_accessibility.h
 *
 * PURPOSE:
 *   Build accessible names, roles, hierarchy, focus and action metadata from
 *   semantic canvas and tree models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_CANVAS_ACCESSIBILITY_H
#define UMICOM_WORKBENCH_DESIGNER_CANVAS_ACCESSIBILITY_H

#include "umicom/workbench_designer/focus_ring.h"
#include "umicom/workbench_designer/tree.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerAccessibleRole {
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_CANVAS = 1,
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_GROUP = 2,
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_TAB_LIST = 3,
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_PANEL = 4,
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_EDITOR = 5,
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_WINDOW = 6
} UmiWorkbenchDesignerAccessibleRole;

typedef enum UmiWorkbenchDesignerAccessibleAction {
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_FOCUS = 1U << 0,
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_SELECT = 1U << 1,
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_OPEN = 1U << 2,
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_MOVE = 1U << 3,
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_RESIZE = 1U << 4,
    UMI_WORKBENCH_DESIGNER_ACCESSIBLE_CLOSE = 1U << 5
} UmiWorkbenchDesignerAccessibleAction;

typedef struct UmiWorkbenchDesignerAccessibleItem {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char description[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    UmiWorkbenchDesignerAccessibleRole role;
    size_t parent_index;
    size_t depth;
    uint32_t action_mask;
    bool selected;
    bool focused;
    bool hidden;
    bool locked;
} UmiWorkbenchDesignerAccessibleItem;

typedef struct UmiWorkbenchDesignerAccessibilityModel {
    UmiWorkbenchDesignerAccessibleItem items[UMI_WORKBENCH_DESIGNER_MAX_CANVAS_ITEMS];
    size_t count;
    uint64_t canvas_revision;
    uint64_t tree_revision;
    uint64_t revision;
} UmiWorkbenchDesignerAccessibilityModel;

void umi_workbench_designer_accessibility_init(UmiWorkbenchDesignerAccessibilityModel *model);
UmiStatus umi_workbench_designer_accessibility_build(UmiWorkbenchDesignerAccessibilityModel *model, const UmiWorkbenchDesignerCanvas *canvas, const UmiWorkbenchDesignerTree *tree, const UmiWorkbenchDesignerFocusRing *focus_ring, bool layout_locked);
const UmiWorkbenchDesignerAccessibleItem *umi_workbench_designer_accessibility_at(const UmiWorkbenchDesignerAccessibilityModel *model, size_t index);
const UmiWorkbenchDesignerAccessibleItem *umi_workbench_designer_accessibility_find(const UmiWorkbenchDesignerAccessibilityModel *model, const char *node_id);

#ifdef __cplusplus
}
#endif

#endif
