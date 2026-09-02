/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/canvas.h
 *
 * PURPOSE:
 *   Project a semantic layout tree into deterministic canvas items with computed
 *   geometry, selection state, hit testing and z-order.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_CANVAS_H
#define UMICOM_WORKBENCH_DESIGNER_CANVAS_H

#include "umicom/workbench_designer/selection.h"
#include "umicom/workbench_designer/viewport.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer canvas item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerCanvasItem {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char title[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char component_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchLayoutNodeKind kind;
    UmiWorkbenchDesignerRect world_bounds;
    UmiWorkbenchDesignerRect screen_bounds;
    size_t node_index;
    size_t depth;
    int32_t z_order;
    bool visible;
    bool selected;
    bool primary;
    bool container;
} UmiWorkbenchDesignerCanvasItem;

/**
 * Represent the workbench designer canvas data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerCanvas {
    UmiWorkbenchDesignerCanvasItem items[UMI_WORKBENCH_DESIGNER_MAX_CANVAS_ITEMS];
    size_t count;
    UmiWorkbenchDesignerRect world_bounds;
    uint64_t document_revision;
    uint64_t viewport_revision;
    uint64_t selection_revision;
    uint64_t revision;
} UmiWorkbenchDesignerCanvas;

/**
 * Initialise workbench designer canvas from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_canvas_init(UmiWorkbenchDesignerCanvas *canvas);
/**
 * Provide the workbench designer canvas build operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_canvas_build(UmiWorkbenchDesignerCanvas *canvas, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerViewport *viewport, const UmiWorkbenchDesignerSelection *selection, UmiWorkbenchDesignerRect root_bounds);
/**
 * Find workbench designer canvas while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_canvas_at(const UmiWorkbenchDesignerCanvas *canvas, size_t index);
/**
 * Find workbench designer canvas while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_canvas_find(const UmiWorkbenchDesignerCanvas *canvas, const char *node_id);
/**
 * Provide the workbench designer canvas hit test operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_canvas_hit_test(const UmiWorkbenchDesignerCanvas *canvas, UmiWorkbenchDesignerPoint screen_point, bool include_containers);
/**
 * Provide the workbench designer canvas selection bounds operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerRect umi_workbench_designer_canvas_selection_bounds(const UmiWorkbenchDesignerCanvas *canvas);

#ifdef __cplusplus
}
#endif

#endif
