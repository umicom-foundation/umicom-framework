/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/layout_designer.h
 *
 * PURPOSE:
 *   Define the transactional visual layout designer owned by Framework. It
 *   clones protected layouts, selects and places windows, applies snapping,
 *   and supplies bounded undo/redo without product-specific geometry logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_LAYOUT_DESIGNER_H
#define UMICOM_DESKTOP_LAYOUT_DESIGNER_H

#include "umicom/desktop/runtime.h"
#include "umicom/desktop/snap_engine.h"

#define UMI_DESKTOP_DESIGNER_HISTORY_DEPTH 16U

typedef enum UmiDesktopResizeEdge {
    UMI_DESKTOP_RESIZE_LEFT = 1,
    UMI_DESKTOP_RESIZE_RIGHT,
    UMI_DESKTOP_RESIZE_TOP,
    UMI_DESKTOP_RESIZE_BOTTOM,
    UMI_DESKTOP_RESIZE_TOP_LEFT,
    UMI_DESKTOP_RESIZE_TOP_RIGHT,
    UMI_DESKTOP_RESIZE_BOTTOM_LEFT,
    UMI_DESKTOP_RESIZE_BOTTOM_RIGHT
} UmiDesktopResizeEdge;

typedef struct UmiDesktopLayoutDesignerSnapshot {
    char source_layout_id[UMI_DESKTOP_ID_CAPACITY];
    char working_layout_id[UMI_DESKTOP_ID_CAPACITY];
    char selected_window_id[UMI_DESKTOP_ID_CAPACITY];
    size_t window_count;
    size_t undo_count;
    size_t redo_count;
    uint32_t snap_guides;
    bool active;
    bool created_copy;
    bool dirty;
    bool can_commit;
    uint64_t revision;
} UmiDesktopLayoutDesignerSnapshot;

typedef struct UmiDesktopLayoutDesigner UmiDesktopLayoutDesigner;

UmiStatus umi_desktop_layout_designer_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopLayoutDesigner **out_designer);
void umi_desktop_layout_designer_destroy(UmiDesktopLayoutDesigner *designer);
UmiStatus umi_desktop_layout_designer_begin(
    UmiDesktopLayoutDesigner *designer,
    const char *source_layout_id,
    const char *working_layout_id,
    const char *working_name);
UmiStatus umi_desktop_layout_designer_select(
    UmiDesktopLayoutDesigner *designer,
    const char *window_id);
UmiStatus umi_desktop_layout_designer_place_selected(
    UmiDesktopLayoutDesigner *designer,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement);
UmiStatus umi_desktop_layout_designer_move_selected(
    UmiDesktopLayoutDesigner *designer,
    int32_t delta_x,
    int32_t delta_y);
UmiStatus umi_desktop_layout_designer_resize_selected(
    UmiDesktopLayoutDesigner *designer,
    UmiDesktopResizeEdge edge,
    int32_t delta_x,
    int32_t delta_y);
UmiStatus umi_desktop_layout_designer_dock_selected(
    UmiDesktopLayoutDesigner *designer,
    UmiDesktopDockPlacement placement);
UmiStatus umi_desktop_layout_designer_undo(
    UmiDesktopLayoutDesigner *designer);
UmiStatus umi_desktop_layout_designer_redo(
    UmiDesktopLayoutDesigner *designer);
UmiStatus umi_desktop_layout_designer_commit(
    UmiDesktopLayoutDesigner *designer);
UmiStatus umi_desktop_layout_designer_cancel(
    UmiDesktopLayoutDesigner *designer);
UmiStatus umi_desktop_layout_designer_set_snap_policy(
    UmiDesktopLayoutDesigner *designer,
    const UmiDesktopSnapPolicy *policy);
UmiStatus umi_desktop_layout_designer_snapshot(
    const UmiDesktopLayoutDesigner *designer,
    UmiDesktopLayoutDesignerSnapshot *out_snapshot);

#endif
