/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/viewport_history.h
 *
 * PURPOSE:
 *   Retain bounded back, forward and named viewport positions for efficient
 *   navigation around large and multi-monitor layout canvases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_VIEWPORT_HISTORY_H
#define UMICOM_WORKBENCH_DESIGNER_VIEWPORT_HISTORY_H

#include "umicom/workbench_designer/viewport.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_VIEWPORT_HISTORY 64U

/**
 * Represent the workbench designer viewport entry data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerViewportEntry {
    char entry_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    UmiWorkbenchDesignerPoint origin;
    UmiWorkbenchDesignerSize canvas_size;
    double zoom;
    uint64_t timestamp_ms;
    bool bookmarked;
} UmiWorkbenchDesignerViewportEntry;

/**
 * Represent the workbench designer viewport history data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerViewportHistory {
    UmiWorkbenchDesignerViewportEntry entries[UMI_WORKBENCH_DESIGNER_MAX_VIEWPORT_HISTORY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiWorkbenchDesignerViewportHistory;

/**
 * Initialise workbench designer viewport history from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_viewport_history_init(UmiWorkbenchDesignerViewportHistory *history);
/**
 * Provide the workbench designer viewport history push operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_viewport_history_push(UmiWorkbenchDesignerViewportHistory *history, const UmiWorkbenchDesignerViewport *viewport, const char *entry_id, const char *label, uint64_t timestamp_ms, bool bookmarked);
/**
 * Provide the workbench designer viewport history can back operation used by this module
 * and its client applications.
 */
bool umi_workbench_designer_viewport_history_can_back(const UmiWorkbenchDesignerViewportHistory *history);
/**
 * Provide the workbench designer viewport history can forward operation used by this
 * module and its client applications.
 */
bool umi_workbench_designer_viewport_history_can_forward(const UmiWorkbenchDesignerViewportHistory *history);
/**
 * Provide the workbench designer viewport history back operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_viewport_history_back(UmiWorkbenchDesignerViewportHistory *history, UmiWorkbenchDesignerViewport *viewport);
/**
 * Provide the workbench designer viewport history forward operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_designer_viewport_history_forward(UmiWorkbenchDesignerViewportHistory *history, UmiWorkbenchDesignerViewport *viewport);
/**
 * Provide the workbench designer viewport history restore operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_designer_viewport_history_restore(const UmiWorkbenchDesignerViewportHistory *history, const char *entry_id, UmiWorkbenchDesignerViewport *viewport);

#ifdef __cplusplus
}
#endif

#endif
