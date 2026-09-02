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

/**
 * Represent the workbench designer focus item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerFocusItem {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    size_t canvas_index;
    uint32_t order;
    bool enabled;
    bool container;
} UmiWorkbenchDesignerFocusItem;

/**
 * Represent the workbench designer focus ring data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerFocusRing {
    UmiWorkbenchDesignerFocusItem items[UMI_WORKBENCH_DESIGNER_MAX_FOCUS_ITEMS];
    size_t count;
    size_t current_index;
    uint64_t canvas_revision;
    uint64_t revision;
} UmiWorkbenchDesignerFocusRing;

/**
 * Initialise workbench designer focus ring from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_focus_ring_init(UmiWorkbenchDesignerFocusRing *ring);
/**
 * Provide the workbench designer focus ring build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_focus_ring_build(UmiWorkbenchDesignerFocusRing *ring, const UmiWorkbenchDesignerCanvas *canvas, bool include_containers);
/**
 * Copy workbench designer focus ring into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_workbench_designer_focus_ring_set(UmiWorkbenchDesignerFocusRing *ring, const char *node_id);
/**
 * Provide the workbench designer focus ring current operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerFocusItem *umi_workbench_designer_focus_ring_current(const UmiWorkbenchDesignerFocusRing *ring);
/**
 * Provide the workbench designer focus ring move operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerFocusItem *umi_workbench_designer_focus_ring_move(UmiWorkbenchDesignerFocusRing *ring, int direction, bool wrap);
/**
 * Find workbench designer focus ring while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWorkbenchDesignerFocusItem *umi_workbench_designer_focus_ring_find(const UmiWorkbenchDesignerFocusRing *ring, const char *node_id);

#ifdef __cplusplus
}
#endif

#endif
