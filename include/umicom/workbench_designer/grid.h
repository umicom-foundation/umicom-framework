/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/grid.h
 *
 * PURPOSE:
 *   Define visible grid lines and coordinate snapping used by the visual
 *   designer canvas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_GRID_H
#define UMICOM_WORKBENCH_DESIGNER_GRID_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer grid data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchDesignerGrid {
    bool enabled;
    bool visible;
    double spacing;
    uint32_t major_every;
    UmiWorkbenchDesignerPoint origin;
    uint64_t revision;
} UmiWorkbenchDesignerGrid;

/**
 * Represent the workbench designer grid line data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerGridLine {
    bool vertical;
    bool major;
    double coordinate;
} UmiWorkbenchDesignerGridLine;

/**
 * Initialise workbench designer grid from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_grid_init(UmiWorkbenchDesignerGrid *grid);
/**
 * Provide the workbench designer grid configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_grid_configure(UmiWorkbenchDesignerGrid *grid, double spacing, uint32_t major_every, bool visible, bool enabled);
/**
 * Provide the workbench designer grid snap value operation used by this module and its
 * client applications.
 */
double umi_workbench_designer_grid_snap_value(const UmiWorkbenchDesignerGrid *grid, double value, double origin);
/**
 * Provide the workbench designer grid snap point operation used by this module and its
 * client applications.
 */
UmiWorkbenchDesignerPoint umi_workbench_designer_grid_snap_point(const UmiWorkbenchDesignerGrid *grid, UmiWorkbenchDesignerPoint point);
/**
 * Return the number of records represented by workbench designer grid line without
 * changing their state.
 */
size_t umi_workbench_designer_grid_line_count(const UmiWorkbenchDesignerGrid *grid, const UmiWorkbenchDesignerRect *visible_world);
/**
 * Find workbench designer grid line while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_workbench_designer_grid_line_at(const UmiWorkbenchDesignerGrid *grid, const UmiWorkbenchDesignerRect *visible_world, size_t index, UmiWorkbenchDesignerGridLine *out_line);

#ifdef __cplusplus
}
#endif

#endif
