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

typedef struct UmiWorkbenchDesignerCanvas {
    UmiWorkbenchDesignerCanvasItem items[UMI_WORKBENCH_DESIGNER_MAX_CANVAS_ITEMS];
    size_t count;
    UmiWorkbenchDesignerRect world_bounds;
    uint64_t document_revision;
    uint64_t viewport_revision;
    uint64_t selection_revision;
    uint64_t revision;
} UmiWorkbenchDesignerCanvas;

void umi_workbench_designer_canvas_init(UmiWorkbenchDesignerCanvas *canvas);
UmiStatus umi_workbench_designer_canvas_build(UmiWorkbenchDesignerCanvas *canvas, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerViewport *viewport, const UmiWorkbenchDesignerSelection *selection, UmiWorkbenchDesignerRect root_bounds);
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_canvas_at(const UmiWorkbenchDesignerCanvas *canvas, size_t index);
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_canvas_find(const UmiWorkbenchDesignerCanvas *canvas, const char *node_id);
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_canvas_hit_test(const UmiWorkbenchDesignerCanvas *canvas, UmiWorkbenchDesignerPoint screen_point, bool include_containers);
UmiWorkbenchDesignerRect umi_workbench_designer_canvas_selection_bounds(const UmiWorkbenchDesignerCanvas *canvas);

#ifdef __cplusplus
}
#endif

#endif
