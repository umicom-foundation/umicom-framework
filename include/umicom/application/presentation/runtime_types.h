/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/runtime_types.h
 *
 * PURPOSE:
 *   Define the live, toolkit-neutral state shared by application presentation
 *   sessions, product controllers and frontend hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_RUNTIME_TYPES_H
#define UMICOM_APPLICATION_PRESENTATION_RUNTIME_TYPES_H

#include "umicom/application/presentation/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_PRESENTATION_MESSAGE_CAPACITY 256U
#define UMI_APPLICATION_PRESENTATION_BADGE_CAPACITY 32U
#define UMI_APPLICATION_PRESENTATION_JOURNAL_CAPACITY 128U

typedef enum UmiApplicationPresentationSurfaceState {
    UMI_APPLICATION_PRESENTATION_STATE_DORMANT = 1,
    UMI_APPLICATION_PRESENTATION_STATE_LOADING = 2,
    UMI_APPLICATION_PRESENTATION_STATE_READY = 3,
    UMI_APPLICATION_PRESENTATION_STATE_EMPTY = 4,
    UMI_APPLICATION_PRESENTATION_STATE_BUSY = 5,
    UMI_APPLICATION_PRESENTATION_STATE_WARNING = 6,
    UMI_APPLICATION_PRESENTATION_STATE_ERROR = 7,
    UMI_APPLICATION_PRESENTATION_STATE_OFFLINE = 8,
    UMI_APPLICATION_PRESENTATION_STATE_PERMISSION_REQUIRED = 9
} UmiApplicationPresentationSurfaceState;

typedef enum UmiApplicationPresentationSurfaceEvent {
    UMI_APPLICATION_PRESENTATION_EVENT_MOUNT = 1,
    UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT = 2,
    UMI_APPLICATION_PRESENTATION_EVENT_ACTIVATE = 3,
    UMI_APPLICATION_PRESENTATION_EVENT_DEACTIVATE = 4,
    UMI_APPLICATION_PRESENTATION_EVENT_FOCUS = 5,
    UMI_APPLICATION_PRESENTATION_EVENT_REFRESH = 6,
    UMI_APPLICATION_PRESENTATION_EVENT_COMMAND = 7,
    UMI_APPLICATION_PRESENTATION_EVENT_CONTEXT_CHANGED = 8
} UmiApplicationPresentationSurfaceEvent;

typedef struct UmiApplicationPresentationSurfaceUpdate {
    UmiApplicationPresentationSurfaceState state;
    char message[UMI_APPLICATION_PRESENTATION_MESSAGE_CAPACITY];
    char badge[UMI_APPLICATION_PRESENTATION_BADGE_CAPACITY];
    uint32_t progress_percent;
    int has_progress;
    int dirty;
} UmiApplicationPresentationSurfaceUpdate;

typedef struct UmiApplicationPresentationSurfaceItem {
    const UmiApplicationPresentationPanelPlacement *placement;
    UmiApplicationPresentationSurfaceState state;
    char message[UMI_APPLICATION_PRESENTATION_MESSAGE_CAPACITY];
    char badge[UMI_APPLICATION_PRESENTATION_BADGE_CAPACITY];
    uint32_t progress_percent;
    int has_progress;
    int visible;
    int focused;
    int dirty;
    uint64_t revision;
} UmiApplicationPresentationSurfaceItem;

typedef struct UmiApplicationPresentationSurfaceSnapshot {
    const char *recipe_id;
    const char *window_id;
    size_t panel_count;
    size_t visible_count;
    size_t ready_count;
    size_t attention_count;
    size_t dirty_count;
    const char *focused_component_id;
    uint64_t revision;
} UmiApplicationPresentationSurfaceSnapshot;

const char *umi_application_presentation_surface_state_text(
    UmiApplicationPresentationSurfaceState state);
const char *umi_application_presentation_surface_event_text(
    UmiApplicationPresentationSurfaceEvent event);

#ifdef __cplusplus
}
#endif

#endif
