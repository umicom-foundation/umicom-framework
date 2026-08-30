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

typedef enum UmiDesktopComponentDragOperation {
    UMI_DESKTOP_COMPONENT_DRAG_MOVE = 1,
    UMI_DESKTOP_COMPONENT_DRAG_COPY = 2,
    UMI_DESKTOP_COMPONENT_DRAG_LINK = 3
} UmiDesktopComponentDragOperation;

typedef enum UmiDesktopComponentDragState {
    UMI_DESKTOP_COMPONENT_DRAG_STARTED = 1,
    UMI_DESKTOP_COMPONENT_DRAG_TARGETED = 2,
    UMI_DESKTOP_COMPONENT_DRAG_COMMITTED = 3,
    UMI_DESKTOP_COMPONENT_DRAG_CANCELLED = 4,
    UMI_DESKTOP_COMPONENT_DRAG_FAILED = 5
} UmiDesktopComponentDragState;

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

typedef struct UmiDesktopComponentDragDrop UmiDesktopComponentDragDrop;

UmiStatus umi_desktop_component_drag_drop_create(
    UmiDesktopRuntime *desktop,
    UmiDesktopContentRuntime *content,
    UmiDesktopComponentDragDrop **out_drag_drop);
void umi_desktop_component_drag_drop_destroy(
    UmiDesktopComponentDragDrop *drag_drop);
UmiUiDragDropRegistry *umi_desktop_component_drag_drop_registry(
    UmiDesktopComponentDragDrop *drag_drop);
UmiStatus umi_desktop_component_drag_begin(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id,
    const char *source_window_id,
    UmiDesktopComponentDragOperation operation);
UmiStatus umi_desktop_component_drag_target(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id,
    const UmiDesktopComponentDropTarget *target);
UmiStatus umi_desktop_component_drag_commit(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id);
UmiStatus umi_desktop_component_drag_cancel(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id);
UmiStatus umi_desktop_component_drag_snapshot(
    const UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id,
    UmiDesktopComponentDragSnapshot *out_snapshot);
size_t umi_desktop_component_drag_count(
    const UmiDesktopComponentDragDrop *drag_drop);

#ifdef __cplusplus
}
#endif

#endif
