/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/viewport_history.h
 *
 * PURPOSE:
 *   Retain bounded back, forward and named viewport positions for efficient
 *   navigation around large and multi-monitor layout canvases.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_VIEWPORT_HISTORY_H
#define UMICOM_WORKBENCH_DESIGNER_VIEWPORT_HISTORY_H

#include "umicom/workbench_designer/viewport.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_VIEWPORT_HISTORY 64U

typedef struct UmiWorkbenchDesignerViewportEntry {
    char entry_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    UmiWorkbenchDesignerPoint origin;
    UmiWorkbenchDesignerSize canvas_size;
    double zoom;
    uint64_t timestamp_ms;
    bool bookmarked;
} UmiWorkbenchDesignerViewportEntry;

typedef struct UmiWorkbenchDesignerViewportHistory {
    UmiWorkbenchDesignerViewportEntry entries[UMI_WORKBENCH_DESIGNER_MAX_VIEWPORT_HISTORY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiWorkbenchDesignerViewportHistory;

void umi_workbench_designer_viewport_history_init(UmiWorkbenchDesignerViewportHistory *history);
UmiStatus umi_workbench_designer_viewport_history_push(UmiWorkbenchDesignerViewportHistory *history, const UmiWorkbenchDesignerViewport *viewport, const char *entry_id, const char *label, uint64_t timestamp_ms, bool bookmarked);
bool umi_workbench_designer_viewport_history_can_back(const UmiWorkbenchDesignerViewportHistory *history);
bool umi_workbench_designer_viewport_history_can_forward(const UmiWorkbenchDesignerViewportHistory *history);
UmiStatus umi_workbench_designer_viewport_history_back(UmiWorkbenchDesignerViewportHistory *history, UmiWorkbenchDesignerViewport *viewport);
UmiStatus umi_workbench_designer_viewport_history_forward(UmiWorkbenchDesignerViewportHistory *history, UmiWorkbenchDesignerViewport *viewport);
UmiStatus umi_workbench_designer_viewport_history_restore(const UmiWorkbenchDesignerViewportHistory *history, const char *entry_id, UmiWorkbenchDesignerViewport *viewport);

#ifdef __cplusplus
}
#endif

#endif
