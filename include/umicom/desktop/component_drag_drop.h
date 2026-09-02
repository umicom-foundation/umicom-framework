/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/component_drag_drop.h
 *
 * PURPOSE:
 *   Define Framework-owned drag-and-drop sessions for hosted desktop
 *   components. Applications declare content; Framework validates and commits
 *   movement, multi-instance copies and context links.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_COMPONENT_DRAG_DROP_H
#define UMICOM_DESKTOP_COMPONENT_DRAG_DROP_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/desktop/content_runtime.h"
#include "umicom/ui/drag_drop.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_COMPONENT_DRAG_MAX 64U

/**
 * List the named desktop component drag operation values accepted by this public contract.
 */
typedef enum UmiDesktopComponentDragOperation {
    UMI_DESKTOP_COMPONENT_DRAG_MOVE = 1,
    UMI_DESKTOP_COMPONENT_DRAG_COPY = 2,
    UMI_DESKTOP_COMPONENT_DRAG_LINK = 3
} UmiDesktopComponentDragOperation;

/**
 * List the named desktop component drag state values accepted by this public contract.
 */
typedef enum UmiDesktopComponentDragState {
    UMI_DESKTOP_COMPONENT_DRAG_STARTED = 1,
    UMI_DESKTOP_COMPONENT_DRAG_TARGETED = 2,
    UMI_DESKTOP_COMPONENT_DRAG_COMMITTED = 3,
    UMI_DESKTOP_COMPONENT_DRAG_CANCELLED = 4,
    UMI_DESKTOP_COMPONENT_DRAG_FAILED = 5
} UmiDesktopComponentDragState;

/**
 * Represent the desktop component drop target data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopComponentDropTarget {
    uint32_t structure_size;
    char target_window_id[UMI_DESKTOP_ID_CAPACITY];
    char monitor_id[UMI_DESKTOP_ID_CAPACITY];
    char new_window_id[UMI_DESKTOP_ID_CAPACITY];
    char new_view_id[UMI_UI_ID_CAPACITY];
    char title[UMI_DESKTOP_TITLE_CAPACITY];
    UmiDesktopRect bounds;
    UmiDesktopDockPlacement placement;
    UmiUiWindowGroupRole context_role;
} UmiDesktopComponentDropTarget;

/**
 * Represent the desktop component drag snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopComponentDragSnapshot {
    uint32_t structure_size;
    char session_id[UMI_DESKTOP_ID_CAPACITY];
    char source_window_id[UMI_DESKTOP_ID_CAPACITY];
    char source_component_id[UMI_DESKTOP_ID_CAPACITY];
    char source_application_id[UMI_DESKTOP_ID_CAPACITY];
    UmiDesktopComponentDragOperation operation;
    UmiDesktopComponentDragState state;
    UmiDesktopComponentDropTarget target;
    int allowed;
    UmiStatus last_status;
    uint64_t revision;
} UmiDesktopComponentDragSnapshot;

/**
 * Represent the desktop component drag drop data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopComponentDragDrop UmiDesktopComponentDragDrop;

/**
 * Initialise desktop component drag drop from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_component_drag_drop_create(
    UmiDesktopRuntime *desktop,
    UmiDesktopContentRuntime *content,
    UmiDesktopComponentDragDrop **out_drag_drop);
/**
 * Release or reset state held by desktop component drag drop so the same storage can be
 * reused safely.
 */
void umi_desktop_component_drag_drop_destroy(
    UmiDesktopComponentDragDrop *drag_drop);
/**
 * Provide the desktop component drag drop registry operation used by this module and its
 * client applications.
 */
UmiUiDragDropRegistry *umi_desktop_component_drag_drop_registry(
    UmiDesktopComponentDragDrop *drag_drop);
/**
 * Provide the desktop component drag begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_component_drag_begin(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id,
    const char *source_window_id,
    UmiDesktopComponentDragOperation operation);
/**
 * Provide the desktop component drag target operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_component_drag_target(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id,
    const UmiDesktopComponentDropTarget *target);
/**
 * Provide the desktop component drag commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_component_drag_commit(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id);
/**
 * Provide the desktop component drag cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_component_drag_cancel(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id);
/**
 * Provide the desktop component drag snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_component_drag_snapshot(
    const UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id,
    UmiDesktopComponentDragSnapshot *out_snapshot);
/**
 * Return the number of records represented by desktop component drag without changing
 * their state.
 */
size_t umi_desktop_component_drag_count(
    const UmiDesktopComponentDragDrop *drag_drop);

#ifdef __cplusplus
}
#endif

#endif
