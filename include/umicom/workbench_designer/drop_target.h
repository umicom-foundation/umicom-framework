/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/drop_target.h
 *
 * PURPOSE:
 *   Enumerate semantic docking targets and select the most specific target under
 *   a pointer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_DROP_TARGET_H
#define UMICOM_WORKBENCH_DESIGNER_DROP_TARGET_H

#include "umicom/workbench_designer/canvas.h"
#include "umicom/workbench_designer/drag.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerDropTarget {
    char target_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char target_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerDropZone zone;
    UmiWorkbenchDesignerRect bounds;
    UmiWorkbenchDesignerRect preview_bounds;
    size_t depth;
    int32_t priority;
    bool enabled;
} UmiWorkbenchDesignerDropTarget;

typedef struct UmiWorkbenchDesignerDropTargetSet {
    UmiWorkbenchDesignerDropTarget targets[UMI_WORKBENCH_DESIGNER_MAX_DROP_TARGETS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerDropTargetSet;

void umi_workbench_designer_drop_targets_init(UmiWorkbenchDesignerDropTargetSet *targets);
UmiStatus umi_workbench_designer_drop_targets_build(UmiWorkbenchDesignerDropTargetSet *targets, const UmiWorkbenchDesignerCanvas *canvas, const char *dragged_node_id);
const UmiWorkbenchDesignerDropTarget *umi_workbench_designer_drop_target_at(const UmiWorkbenchDesignerDropTargetSet *targets, size_t index);
const UmiWorkbenchDesignerDropTarget *umi_workbench_designer_drop_target_hit_test(const UmiWorkbenchDesignerDropTargetSet *targets, UmiWorkbenchDesignerPoint point);

#ifdef __cplusplus
}
#endif

#endif
