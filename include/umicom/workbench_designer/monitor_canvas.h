/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/monitor_canvas.h
 *
 * PURPOSE:
 *   Describe multi-monitor work areas and safe placement for floating semantic
 *   windows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_MONITOR_CANVAS_H
#define UMICOM_WORKBENCH_DESIGNER_MONITOR_CANVAS_H

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer monitor data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerMonitor {
    char monitor_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    UmiWorkbenchDesignerRect bounds;
    UmiWorkbenchDesignerRect work_area;
    double scale;
    bool primary;
    bool enabled;
} UmiWorkbenchDesignerMonitor;

/**
 * Represent the workbench designer monitor canvas data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerMonitorCanvas {
    UmiWorkbenchDesignerMonitor monitors[UMI_WORKBENCH_DESIGNER_MAX_MONITORS];
    size_t count;
    char primary_monitor_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerRect combined_bounds;
    uint64_t revision;
} UmiWorkbenchDesignerMonitorCanvas;

/**
 * Initialise workbench designer monitor canvas from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_monitor_canvas_init(UmiWorkbenchDesignerMonitorCanvas *canvas);
/**
 * Add workbench designer monitor canvas only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_designer_monitor_canvas_add(UmiWorkbenchDesignerMonitorCanvas *canvas, const UmiWorkbenchDesignerMonitor *monitor);
/**
 * Remove workbench designer monitor canvas while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_designer_monitor_canvas_remove(UmiWorkbenchDesignerMonitorCanvas *canvas, const char *monitor_id);
/**
 * Find workbench designer monitor canvas while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerMonitor *umi_workbench_designer_monitor_canvas_find(const UmiWorkbenchDesignerMonitorCanvas *canvas, const char *monitor_id);
/**
 * Provide the workbench designer monitor canvas primary operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerMonitor *umi_workbench_designer_monitor_canvas_primary(const UmiWorkbenchDesignerMonitorCanvas *canvas);
/**
 * Provide the workbench designer monitor canvas clamp operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerRect umi_workbench_designer_monitor_canvas_clamp(const UmiWorkbenchDesignerMonitorCanvas *canvas, const char *monitor_id, UmiWorkbenchDesignerRect bounds);
/**
 * Provide the workbench designer monitor canvas relocate missing operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_designer_monitor_canvas_relocate_missing(UmiWorkbenchDesignerMonitorCanvas *canvas, UmiWorkbenchLayoutDocument *document);

#ifdef __cplusplus
}
#endif

#endif
