/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/minimap.h
 *
 * PURPOSE:
 *   Project semantic canvas items and the visible viewport into a compact
 *   minimap model suitable for any frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_MINIMAP_H
#define UMICOM_WORKBENCH_DESIGNER_MINIMAP_H

#include "umicom/workbench_designer/canvas.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer minimap item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerMinimapItem {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerRect bounds;
    UmiWorkbenchLayoutNodeKind kind;
    bool selected;
    bool active;
} UmiWorkbenchDesignerMinimapItem;

/**
 * Represent the workbench designer minimap data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerMinimap {
    UmiWorkbenchDesignerMinimapItem items[UMI_WORKBENCH_DESIGNER_MAX_CANVAS_ITEMS];
    size_t count;
    UmiWorkbenchDesignerRect content_bounds;
    UmiWorkbenchDesignerRect viewport_bounds;
    UmiWorkbenchDesignerSize surface_size;
    double scale;
    uint64_t canvas_revision;
    uint64_t viewport_revision;
    uint64_t revision;
} UmiWorkbenchDesignerMinimap;

/**
 * Initialise workbench designer minimap from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_minimap_init(UmiWorkbenchDesignerMinimap *minimap);
/**
 * Provide the workbench designer minimap build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_minimap_build(
    UmiWorkbenchDesignerMinimap *minimap,
    const UmiWorkbenchDesignerCanvas *canvas,
    const UmiWorkbenchDesignerViewport *viewport,
    UmiWorkbenchDesignerSize surface_size);
/**
 * Provide the workbench designer minimap world point operation used by this module and its
 * client applications.
 */
UmiWorkbenchDesignerPoint umi_workbench_designer_minimap_world_point(
    const UmiWorkbenchDesignerMinimap *minimap,
    UmiWorkbenchDesignerPoint minimap_point);

#ifdef __cplusplus
}
#endif

#endif
