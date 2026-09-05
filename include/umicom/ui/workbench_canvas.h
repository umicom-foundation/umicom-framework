/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workbench_canvas.h
 *
 * PURPOSE:
 *   Coordinate one or more Framework-owned Workbench Host canvases without
 *   depending on GTK, Qt, a web view or another frontend toolkit.
 *
 *   The canvas keeps workspace customisation as the single layout owner and
 *   adds host, monitor and detachable-window state for frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKBENCH_CANVAS_H
#define UMICOM_UI_WORKBENCH_CANVAS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/ui/workspace_customisation.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bound host and monitor state so plugins cannot allocate untrusted amounts. */
#define UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS 8U
#define UMI_UI_WORKBENCH_CANVAS_MONITOR_ID_CAPACITY UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY

/** Describe one surface's detached-window state. */
typedef struct UmiUiWorkbenchCanvasSurfaceState {
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char monitor_id[UMI_UI_WORKBENCH_CANVAS_MONITOR_ID_CAPACITY];
    bool detached;
    bool visible;
    uint64_t revision;
} UmiUiWorkbenchCanvasSurfaceState;

/** Describe one native Workbench Host window and its borrowed layout model. */
typedef struct UmiUiWorkbenchCanvasHost {
    char host_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char application_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char monitor_id[UMI_UI_WORKBENCH_CANVAS_MONITOR_ID_CAPACITY];
    UmiUiWorkspaceCustomisation *customisation;
    UmiUiWorkbenchCanvasSurfaceState surfaces[UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS];
    size_t surface_count;
    uint64_t revision;
    bool active;
} UmiUiWorkbenchCanvasHost;

/** Own the bounded set of host registrations for one application session. */
typedef struct UmiUiWorkbenchCanvas {
    UmiUiWorkbenchCanvasHost hosts[UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS];
    size_t host_count;
    size_t active_host_index;
    uint64_t revision;
} UmiUiWorkbenchCanvas;

/** Provide an immutable summary for frontend menus, status bars and renderers. */
typedef struct UmiUiWorkbenchCanvasHostSnapshot {
    char host_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char application_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char monitor_id[UMI_UI_WORKBENCH_CANVAS_MONITOR_ID_CAPACITY];
    char active_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    size_t surface_count;
    bool active;
    bool layout_locked;
    uint64_t revision;
} UmiUiWorkbenchCanvasHostSnapshot;

/** Provide an immutable summary for the complete canvas session. */
typedef struct UmiUiWorkbenchCanvasSnapshot {
    size_t host_count;
    size_t active_host_index;
    uint64_t revision;
    UmiUiWorkbenchCanvasHostSnapshot hosts[UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS];
} UmiUiWorkbenchCanvasSnapshot;

/* Initialise an empty canvas before hosts are registered. */
void umi_ui_workbench_canvas_init(UmiUiWorkbenchCanvas *canvas);

/* Register a host and its Framework-owned customisation model. */
UmiStatus umi_ui_workbench_canvas_add_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *application_id,
    const char *monitor_id,
    UmiUiWorkspaceCustomisation *customisation);

/* Unregister a native host after its window closes. The borrowed
 * customisation model is never freed; remaining hosts keep a valid active
 * selection and the canvas revision advances once. */
UmiStatus umi_ui_workbench_canvas_remove_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id);

/* Load one registered application experience and attach its product layout to a
 * new host in one operation. This is the common entry point used by Studio,
 * Trader and the other Umicom applications so each product receives the same
 * Framework-owned layout lifecycle. The implementation belongs to the
 * application-suite layout target; the UI target only owns the lower-level
 * canvas mechanics, which keeps static-library dependencies one-way. */
UmiStatus umi_ui_workbench_canvas_add_application_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *application_id,
    const char *monitor_id,
    UmiUiWorkspaceCustomisation *customisation);

/* Select which host receives commands that do not name a host explicitly. */
UmiStatus umi_ui_workbench_canvas_set_active_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id);

/* Return a mutable registered host or NULL when its identity is unknown. */
UmiUiWorkbenchCanvasHost *umi_ui_workbench_canvas_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id);

/* Return an immutable registered host or NULL when its identity is unknown. */
const UmiUiWorkbenchCanvasHost *umi_ui_workbench_canvas_host_const(
    const UmiUiWorkbenchCanvas *canvas,
    const char *host_id);

/* Create and activate a named layout containing no removable surfaces. */
UmiStatus umi_ui_workbench_canvas_create_blank_layout(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *layout_id,
    const char *name);

/* Remove every closable surface from the active layout as one edit session. */
UmiStatus umi_ui_workbench_canvas_clear_current_canvas(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id);

/* Open a catalogue surface through the existing customisation transaction. */
UmiStatus umi_ui_workbench_canvas_open_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *tool_id,
    const char *group_id,
    bool floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity);

/* Move a surface and make it floating inside the normalised canvas. */
UmiStatus umi_ui_workbench_canvas_move_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *window_id,
    double x,
    double y);

/* Resize a surface while preserving its current top-left position. */
UmiStatus umi_ui_workbench_canvas_resize_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *window_id,
    double width,
    double height);

/* Snap a floating surface to a positive normalised grid step. */
UmiStatus umi_ui_workbench_canvas_snap_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *window_id,
    double grid_step);

/* Detach a surface into a native window associated with another monitor. */
UmiStatus umi_ui_workbench_canvas_detach_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *window_id,
    const char *monitor_id);

/* Reattach a detached surface to a dock region and tab stack in its host. */
UmiStatus umi_ui_workbench_canvas_attach_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *window_id,
    const char *placement_id,
    const char *stack_id);

/* Lock or unlock the active layout through the canonical layout contract. */
UmiStatus umi_ui_workbench_canvas_set_layout_locked(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    bool locked);

/* Return detached/monitor state for one known surface. */
const UmiUiWorkbenchCanvasSurfaceState *umi_ui_workbench_canvas_surface_state(
    const UmiUiWorkbenchCanvasHost *host,
    const char *window_id);

/* Copy a bounded host summary for menus, status bars and accessibility views. */
UmiStatus umi_ui_workbench_canvas_snapshot(
    const UmiUiWorkbenchCanvas *canvas,
    UmiUiWorkbenchCanvasSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
