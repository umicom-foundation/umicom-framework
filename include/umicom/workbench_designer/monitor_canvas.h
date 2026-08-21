/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/monitor_canvas.h
 *
 * PURPOSE:
 *   Describe multi-monitor work areas and safe placement for floating semantic
 *   windows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_MONITOR_CANVAS_H
#define UMICOM_WORKBENCH_DESIGNER_MONITOR_CANVAS_H

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerMonitor {
    char monitor_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    UmiWorkbenchDesignerRect bounds;
    UmiWorkbenchDesignerRect work_area;
    double scale;
    bool primary;
    bool enabled;
} UmiWorkbenchDesignerMonitor;

typedef struct UmiWorkbenchDesignerMonitorCanvas {
    UmiWorkbenchDesignerMonitor monitors[UMI_WORKBENCH_DESIGNER_MAX_MONITORS];
    size_t count;
    char primary_monitor_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerRect combined_bounds;
    uint64_t revision;
} UmiWorkbenchDesignerMonitorCanvas;

void umi_workbench_designer_monitor_canvas_init(UmiWorkbenchDesignerMonitorCanvas *canvas);
UmiStatus umi_workbench_designer_monitor_canvas_add(UmiWorkbenchDesignerMonitorCanvas *canvas, const UmiWorkbenchDesignerMonitor *monitor);
UmiStatus umi_workbench_designer_monitor_canvas_remove(UmiWorkbenchDesignerMonitorCanvas *canvas, const char *monitor_id);
const UmiWorkbenchDesignerMonitor *umi_workbench_designer_monitor_canvas_find(const UmiWorkbenchDesignerMonitorCanvas *canvas, const char *monitor_id);
const UmiWorkbenchDesignerMonitor *umi_workbench_designer_monitor_canvas_primary(const UmiWorkbenchDesignerMonitorCanvas *canvas);
UmiWorkbenchDesignerRect umi_workbench_designer_monitor_canvas_clamp(const UmiWorkbenchDesignerMonitorCanvas *canvas, const char *monitor_id, UmiWorkbenchDesignerRect bounds);
UmiStatus umi_workbench_designer_monitor_canvas_relocate_missing(UmiWorkbenchDesignerMonitorCanvas *canvas, UmiWorkbenchLayoutDocument *document);

#ifdef __cplusplus
}
#endif

#endif
