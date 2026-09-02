/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/snap.h
 *
 * PURPOSE:
 *   Calculate deterministic grid, edge and centre alignment for drag and resize
 *   previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_SNAP_H
#define UMICOM_WORKBENCH_DESIGNER_SNAP_H

#include "umicom/workbench_designer/grid.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer snap settings data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerSnapSettings {
    bool snap_to_grid;
    bool snap_to_edges;
    bool snap_to_centres;
    double tolerance;
} UmiWorkbenchDesignerSnapSettings;

/**
 * Represent the workbench designer snap guide data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerSnapGuide {
    bool vertical;
    double coordinate;
    char source_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
} UmiWorkbenchDesignerSnapGuide;

/**
 * Represent the workbench designer snap result data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerSnapResult {
    UmiWorkbenchDesignerRect bounds;
    UmiWorkbenchDesignerSnapGuide guides[UMI_WORKBENCH_DESIGNER_MAX_GUIDES];
    size_t guide_count;
    bool snapped_x;
    bool snapped_y;
} UmiWorkbenchDesignerSnapResult;

/**
 * Provide the workbench designer snap settings default operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerSnapSettings umi_workbench_designer_snap_settings_default(void);
/**
 * Initialise workbench designer snap result from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_snap_result_init(UmiWorkbenchDesignerSnapResult *result, UmiWorkbenchDesignerRect bounds);
/**
 * Provide the workbench designer snap rect operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_snap_rect(const UmiWorkbenchDesignerSnapSettings *settings, const UmiWorkbenchDesignerGrid *grid, const UmiWorkbenchDesignerRect *candidate, const UmiWorkbenchDesignerRect *other_bounds, const char *const *other_ids, size_t other_count, UmiWorkbenchDesignerSnapResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
