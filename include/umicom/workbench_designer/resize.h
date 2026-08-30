/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/resize.h
 *
 * PURPOSE:
 *   Model resize handles and calculate constrained node geometry for the visual
 *   layout canvas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_RESIZE_H
#define UMICOM_WORKBENCH_DESIGNER_RESIZE_H

#include "umicom/workbench_designer/canvas.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerResizeHandle {
    UMI_WORKBENCH_DESIGNER_RESIZE_NONE = 0,
    UMI_WORKBENCH_DESIGNER_RESIZE_NORTH = 1,
    UMI_WORKBENCH_DESIGNER_RESIZE_NORTH_EAST = 2,
    UMI_WORKBENCH_DESIGNER_RESIZE_EAST = 3,
    UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH_EAST = 4,
    UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH = 5,
    UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH_WEST = 6,
    UMI_WORKBENCH_DESIGNER_RESIZE_WEST = 7,
    UMI_WORKBENCH_DESIGNER_RESIZE_NORTH_WEST = 8
} UmiWorkbenchDesignerResizeHandle;

typedef struct UmiWorkbenchDesignerResizePolicy {
    UmiWorkbenchDesignerSize minimum_size;
    UmiWorkbenchDesignerSize maximum_size;
    bool preserve_aspect_ratio;
    bool clamp_to_canvas;
    double handle_size;
} UmiWorkbenchDesignerResizePolicy;

typedef struct UmiWorkbenchDesignerResizeSession {
    UmiWorkbenchDesignerResizeHandle handle;
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerPoint start_point;
    UmiWorkbenchDesignerRect original_bounds;
    UmiWorkbenchDesignerRect preview_bounds;
    UmiWorkbenchDesignerResizePolicy policy;
    bool active;
    uint64_t revision;
} UmiWorkbenchDesignerResizeSession;

UmiWorkbenchDesignerResizePolicy umi_workbench_designer_resize_policy_default(void);
void umi_workbench_designer_resize_init(UmiWorkbenchDesignerResizeSession *session);
UmiStatus umi_workbench_designer_resize_begin(
    UmiWorkbenchDesignerResizeSession *session,
    const char *node_id,
    UmiWorkbenchDesignerResizeHandle handle,
    UmiWorkbenchDesignerPoint pointer,
    UmiWorkbenchDesignerRect bounds,
    const UmiWorkbenchDesignerResizePolicy *policy);
UmiStatus umi_workbench_designer_resize_update(
    UmiWorkbenchDesignerResizeSession *session,
    UmiWorkbenchDesignerPoint pointer,
    UmiWorkbenchDesignerRect canvas_bounds);
UmiStatus umi_workbench_designer_resize_commit(
    UmiWorkbenchDesignerResizeSession *session,
    UmiWorkbenchLayoutDocument *document);
void umi_workbench_designer_resize_cancel(UmiWorkbenchDesignerResizeSession *session);
UmiWorkbenchDesignerResizeHandle umi_workbench_designer_resize_hit_test(
    UmiWorkbenchDesignerRect bounds,
    UmiWorkbenchDesignerPoint point,
    double handle_size);

#ifdef __cplusplus
}
#endif

#endif
