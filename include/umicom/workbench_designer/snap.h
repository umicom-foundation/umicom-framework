/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/snap.h
 *
 * PURPOSE:
 *   Calculate deterministic grid, edge and centre alignment for drag and resize
 *   previews.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_SNAP_H
#define UMICOM_WORKBENCH_DESIGNER_SNAP_H

#include "umicom/workbench_designer/grid.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerSnapSettings {
    bool snap_to_grid;
    bool snap_to_edges;
    bool snap_to_centres;
    double tolerance;
} UmiWorkbenchDesignerSnapSettings;

typedef struct UmiWorkbenchDesignerSnapGuide {
    bool vertical;
    double coordinate;
    char source_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
} UmiWorkbenchDesignerSnapGuide;

typedef struct UmiWorkbenchDesignerSnapResult {
    UmiWorkbenchDesignerRect bounds;
    UmiWorkbenchDesignerSnapGuide guides[UMI_WORKBENCH_DESIGNER_MAX_GUIDES];
    size_t guide_count;
    bool snapped_x;
    bool snapped_y;
} UmiWorkbenchDesignerSnapResult;

UmiWorkbenchDesignerSnapSettings umi_workbench_designer_snap_settings_default(void);
void umi_workbench_designer_snap_result_init(UmiWorkbenchDesignerSnapResult *result, UmiWorkbenchDesignerRect bounds);
UmiStatus umi_workbench_designer_snap_rect(const UmiWorkbenchDesignerSnapSettings *settings, const UmiWorkbenchDesignerGrid *grid, const UmiWorkbenchDesignerRect *candidate, const UmiWorkbenchDesignerRect *other_bounds, const char *const *other_ids, size_t other_count, UmiWorkbenchDesignerSnapResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
