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

/**
 * List the named desktop resize edge values accepted by this public contract.
 */
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

/**
 * Represent the desktop layout designer snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the desktop layout designer data shared with callers of this public contract.
 */
typedef struct UmiDesktopLayoutDesigner UmiDesktopLayoutDesigner;

/**
 * Initialise desktop layout designer from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_layout_designer_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopLayoutDesigner **out_designer);
/**
 * Release or reset state held by desktop layout designer so the same storage can be reused
 * safely.
 */
void umi_desktop_layout_designer_destroy(UmiDesktopLayoutDesigner *designer);
/**
 * Provide the desktop layout designer begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_begin(
    UmiDesktopLayoutDesigner *designer,
    const char *source_layout_id,
    const char *working_layout_id,
    const char *working_name);
/**
 * Provide the desktop layout designer select operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_select(
    UmiDesktopLayoutDesigner *designer,
    const char *window_id);
/**
 * Find desktop layout designer place while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_desktop_layout_designer_place_selected(
    UmiDesktopLayoutDesigner *designer,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement);
/**
 * Find desktop layout designer move while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_desktop_layout_designer_move_selected(
    UmiDesktopLayoutDesigner *designer,
    int32_t delta_x,
    int32_t delta_y);
/**
 * Find desktop layout designer resize while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_desktop_layout_designer_resize_selected(
    UmiDesktopLayoutDesigner *designer,
    UmiDesktopResizeEdge edge,
    int32_t delta_x,
    int32_t delta_y);
/**
 * Find desktop layout designer dock while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_desktop_layout_designer_dock_selected(
    UmiDesktopLayoutDesigner *designer,
    UmiDesktopDockPlacement placement);
/**
 * Provide the desktop layout designer undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_undo(
    UmiDesktopLayoutDesigner *designer);
/**
 * Provide the desktop layout designer redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_redo(
    UmiDesktopLayoutDesigner *designer);
/**
 * Provide the desktop layout designer commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_commit(
    UmiDesktopLayoutDesigner *designer);
/**
 * Provide the desktop layout designer cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_cancel(
    UmiDesktopLayoutDesigner *designer);
/**
 * Provide the desktop layout designer set snap policy operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_layout_designer_set_snap_policy(
    UmiDesktopLayoutDesigner *designer,
    const UmiDesktopSnapPolicy *policy);
/**
 * Provide the desktop layout designer snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_layout_designer_snapshot(
    const UmiDesktopLayoutDesigner *designer,
    UmiDesktopLayoutDesignerSnapshot *out_snapshot);

#endif
