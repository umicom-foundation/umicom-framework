/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/drag.h
 *
 * PURPOSE:
 *   Represent pointer-independent drag lifecycle, preview geometry and
 *   cancellation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_DRAG_H
#define UMICOM_WORKBENCH_DESIGNER_DRAG_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench designer drag phase values accepted by this public contract.
 */
typedef enum UmiWorkbenchDesignerDragPhase {
    UMI_WORKBENCH_DESIGNER_DRAG_IDLE = 1,
    UMI_WORKBENCH_DESIGNER_DRAG_ARMED = 2,
    UMI_WORKBENCH_DESIGNER_DRAG_ACTIVE = 3,
    UMI_WORKBENCH_DESIGNER_DRAG_COMPLETED = 4,
    UMI_WORKBENCH_DESIGNER_DRAG_CANCELLED = 5
} UmiWorkbenchDesignerDragPhase;

/**
 * Represent the workbench designer drag session data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerDragSession {
    UmiWorkbenchDesignerDragPhase phase;
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerPoint start_point;
    UmiWorkbenchDesignerPoint current_point;
    UmiWorkbenchDesignerPoint pointer_offset;
    UmiWorkbenchDesignerRect original_bounds;
    UmiWorkbenchDesignerRect preview_bounds;
    UmiWorkbenchDesignerDropZone proposed_zone;
    char proposed_target_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t started_at_ms;
    uint64_t updated_at_ms;
    uint64_t revision;
} UmiWorkbenchDesignerDragSession;

/**
 * Initialise workbench designer drag from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_drag_init(UmiWorkbenchDesignerDragSession *drag);
/**
 * Provide the workbench designer drag begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_drag_begin(UmiWorkbenchDesignerDragSession *drag, const char *node_id, UmiWorkbenchDesignerPoint pointer, UmiWorkbenchDesignerRect bounds, uint64_t timestamp_ms);
/**
 * Provide the workbench designer drag update operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_drag_update(UmiWorkbenchDesignerDragSession *drag, UmiWorkbenchDesignerPoint pointer, uint64_t timestamp_ms);
/**
 * Provide the workbench designer drag set target operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_drag_set_target(UmiWorkbenchDesignerDragSession *drag, const char *target_node_id, UmiWorkbenchDesignerDropZone zone);
/**
 * Provide the workbench designer drag complete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_drag_complete(UmiWorkbenchDesignerDragSession *drag);
/**
 * Provide the workbench designer drag cancel operation used by this module and its client
 * applications.
 */
void umi_workbench_designer_drag_cancel(UmiWorkbenchDesignerDragSession *drag);

#ifdef __cplusplus
}
#endif

#endif
