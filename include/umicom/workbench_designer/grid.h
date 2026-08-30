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


typedef struct UmiWorkbenchDesignerGrid {
    bool enabled;
    bool visible;
    double spacing;
    uint32_t major_every;
    UmiWorkbenchDesignerPoint origin;
    uint64_t revision;
} UmiWorkbenchDesignerGrid;

typedef struct UmiWorkbenchDesignerGridLine {
    bool vertical;
    bool major;
    double coordinate;
} UmiWorkbenchDesignerGridLine;

void umi_workbench_designer_grid_init(UmiWorkbenchDesignerGrid *grid);
UmiStatus umi_workbench_designer_grid_configure(UmiWorkbenchDesignerGrid *grid, double spacing, uint32_t major_every, bool visible, bool enabled);
double umi_workbench_designer_grid_snap_value(const UmiWorkbenchDesignerGrid *grid, double value, double origin);
UmiWorkbenchDesignerPoint umi_workbench_designer_grid_snap_point(const UmiWorkbenchDesignerGrid *grid, UmiWorkbenchDesignerPoint point);
size_t umi_workbench_designer_grid_line_count(const UmiWorkbenchDesignerGrid *grid, const UmiWorkbenchDesignerRect *visible_world);
UmiStatus umi_workbench_designer_grid_line_at(const UmiWorkbenchDesignerGrid *grid, const UmiWorkbenchDesignerRect *visible_world, size_t index, UmiWorkbenchDesignerGridLine *out_line);

#ifdef __cplusplus
}
#endif

#endif
