/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/placement.h
 *
 * PURPOSE:
 *   Restore, clamp, cascade and relocate semantic window geometry when monitor topology changes or a layout moves between computers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_PLACEMENT_H
#define UMICOM_WORKBENCH_LAYOUT_PLACEMENT_H

#include "umicom/workbench_layout/document.h"
#include "umicom/workbench_layout/monitor_topology.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout placement options data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutPlacementOptions {
    uint32_t structure_size;
    int32_t cascade_offset_x;
    int32_t cascade_offset_y;
    UmiWorkbenchLayoutSize default_size;
    bool preserve_size;
    bool prefer_recorded_monitor;
    bool clamp_to_work_area;
    bool relocate_offscreen_windows;
} UmiWorkbenchLayoutPlacementOptions;

/**
 * Represent the workbench layout placement report data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutPlacementReport {
    uint32_t structure_size;
    size_t inspected_count;
    size_t relocated_count;
    size_t clamped_count;
    size_t resized_count;
    size_t missing_monitor_count;
    bool changed;
} UmiWorkbenchLayoutPlacementReport;

/**
 * Provide the workbench layout placement options default operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutPlacementOptions
umi_workbench_layout_placement_options_default(void);

/**
 * Provide the workbench layout place document operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_place_document(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchLayoutPlacementOptions *options,
    UmiWorkbenchLayoutPlacementReport *out_report);

/**
 * Provide the workbench layout place node operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_place_node(
    UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchLayoutPlacementOptions *options,
    size_t cascade_index,
    bool *out_changed);

/**
 * Provide the workbench layout clamp rect operation used by this module and its client
 * applications.
 */
UmiWorkbenchLayoutRect umi_workbench_layout_clamp_rect(
    const UmiWorkbenchLayoutRect *rect,
    const UmiWorkbenchLayoutRect *work_area,
    const UmiWorkbenchLayoutSize *minimum_size);

/**
 * Provide the workbench layout cascade rect operation used by this module and its client
 * applications.
 */
UmiWorkbenchLayoutRect umi_workbench_layout_cascade_rect(
    const UmiWorkbenchLayoutRect *work_area,
    const UmiWorkbenchLayoutSize *size,
    int32_t offset_x,
    int32_t offset_y,
    size_t cascade_index);

#ifdef __cplusplus
}
#endif

#endif
