/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/drag.h
 *
 * PURPOSE:
 *   Represent pointer-independent drag lifecycle, preview geometry and
 *   cancellation evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_DRAG_H
#define UMICOM_WORKBENCH_DESIGNER_DRAG_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerDragPhase {
    UMI_WORKBENCH_DESIGNER_DRAG_IDLE = 1,
    UMI_WORKBENCH_DESIGNER_DRAG_ARMED = 2,
    UMI_WORKBENCH_DESIGNER_DRAG_ACTIVE = 3,
    UMI_WORKBENCH_DESIGNER_DRAG_COMPLETED = 4,
    UMI_WORKBENCH_DESIGNER_DRAG_CANCELLED = 5
} UmiWorkbenchDesignerDragPhase;

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

void umi_workbench_designer_drag_init(UmiWorkbenchDesignerDragSession *drag);
UmiStatus umi_workbench_designer_drag_begin(UmiWorkbenchDesignerDragSession *drag, const char *node_id, UmiWorkbenchDesignerPoint pointer, UmiWorkbenchDesignerRect bounds, uint64_t timestamp_ms);
UmiStatus umi_workbench_designer_drag_update(UmiWorkbenchDesignerDragSession *drag, UmiWorkbenchDesignerPoint pointer, uint64_t timestamp_ms);
UmiStatus umi_workbench_designer_drag_set_target(UmiWorkbenchDesignerDragSession *drag, const char *target_node_id, UmiWorkbenchDesignerDropZone zone);
UmiStatus umi_workbench_designer_drag_complete(UmiWorkbenchDesignerDragSession *drag);
void umi_workbench_designer_drag_cancel(UmiWorkbenchDesignerDragSession *drag);

#ifdef __cplusplus
}
#endif

#endif
