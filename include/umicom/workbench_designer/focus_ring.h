/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/focus_ring.h
 *
 * PURPOSE:
 *   Provide deterministic keyboard focus order for canvas items so GTK4, Qt, web
 *   and accessibility adapters navigate the same semantic nodes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_FOCUS_RING_H
#define UMICOM_WORKBENCH_DESIGNER_FOCUS_RING_H

#include "umicom/workbench_designer/canvas.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_FOCUS_ITEMS 256U

typedef struct UmiWorkbenchDesignerFocusItem {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    size_t canvas_index;
    uint32_t order;
    bool enabled;
    bool container;
} UmiWorkbenchDesignerFocusItem;

typedef struct UmiWorkbenchDesignerFocusRing {
    UmiWorkbenchDesignerFocusItem items[UMI_WORKBENCH_DESIGNER_MAX_FOCUS_ITEMS];
    size_t count;
    size_t current_index;
    uint64_t canvas_revision;
    uint64_t revision;
} UmiWorkbenchDesignerFocusRing;

void umi_workbench_designer_focus_ring_init(UmiWorkbenchDesignerFocusRing *ring);
UmiStatus umi_workbench_designer_focus_ring_build(UmiWorkbenchDesignerFocusRing *ring, const UmiWorkbenchDesignerCanvas *canvas, bool include_containers);
UmiStatus umi_workbench_designer_focus_ring_set(UmiWorkbenchDesignerFocusRing *ring, const char *node_id);
const UmiWorkbenchDesignerFocusItem *umi_workbench_designer_focus_ring_current(const UmiWorkbenchDesignerFocusRing *ring);
const UmiWorkbenchDesignerFocusItem *umi_workbench_designer_focus_ring_move(UmiWorkbenchDesignerFocusRing *ring, int direction, bool wrap);
const UmiWorkbenchDesignerFocusItem *umi_workbench_designer_focus_ring_find(const UmiWorkbenchDesignerFocusRing *ring, const char *node_id);

#ifdef __cplusplus
}
#endif

#endif
