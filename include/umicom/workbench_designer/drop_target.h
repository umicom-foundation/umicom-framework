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


/**
 * Represent the workbench designer drop target data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the workbench designer drop target set data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerDropTargetSet {
    UmiWorkbenchDesignerDropTarget targets[UMI_WORKBENCH_DESIGNER_MAX_DROP_TARGETS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerDropTargetSet;

/**
 * Initialise workbench designer drop targets from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_drop_targets_init(UmiWorkbenchDesignerDropTargetSet *targets);
/**
 * Provide the workbench designer drop targets build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_drop_targets_build(UmiWorkbenchDesignerDropTargetSet *targets, const UmiWorkbenchDesignerCanvas *canvas, const char *dragged_node_id);
/**
 * Find workbench designer drop target while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerDropTarget *umi_workbench_designer_drop_target_at(const UmiWorkbenchDesignerDropTargetSet *targets, size_t index);
/**
 * Provide the workbench designer drop target hit test operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerDropTarget *umi_workbench_designer_drop_target_hit_test(const UmiWorkbenchDesignerDropTargetSet *targets, UmiWorkbenchDesignerPoint point);

#ifdef __cplusplus
}
#endif

#endif
