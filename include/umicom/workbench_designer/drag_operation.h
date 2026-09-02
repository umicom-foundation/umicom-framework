/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/drag_operation.h
 *
 * PURPOSE:
 *   Translate a completed drag and semantic drop target into one auditable
 *   Layout Designer command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_DRAG_OPERATION_H
#define UMICOM_WORKBENCH_DESIGNER_DRAG_OPERATION_H

#include "umicom/workbench_designer/command.h"
#include "umicom/workbench_designer/drop_target.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer drag operation plan data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerDragOperationPlan {
    UmiWorkbenchDesignerCommand command;
    UmiWorkbenchDesignerRect preview_bounds;
    bool creates_container;
    bool creates_tab_group;
    bool valid;
    char explanation[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
} UmiWorkbenchDesignerDragOperationPlan;

/**
 * Provide the workbench designer drag operation plan operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_drag_operation_plan(
    const UmiWorkbenchDesignerDragSession *drag,
    const UmiWorkbenchDesignerDropTarget *target,
    const char *command_id,
    const char *actor_id,
    uint64_t expected_revision,
    UmiWorkbenchDesignerDragOperationPlan *out_plan);

#ifdef __cplusplus
}
#endif

#endif
