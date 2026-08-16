/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/monitor_canvas.h
 *
 * PURPOSE:
 *   Project the Framework-owned physical monitor topology and desktop windows
 *   into a toolkit-neutral visual canvas. GTK4, Qt, Wt and headless frontends
 *   all consume the same scale, offsets and hit-testing rules.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_MONITOR_CANVAS_H
#define UMICOM_DESKTOP_MONITOR_CANVAS_H

#include "umicom/desktop/window_manager.h"

typedef struct UmiDesktopCanvasRect {
    double x;
    double y;
    double width;
    double height;
} UmiDesktopCanvasRect;

typedef struct UmiDesktopMonitorCanvas {
    UmiDesktopRect desktop_bounds;
    UmiDesktopCanvasRect viewport;
    double scale;
    double offset_x;
    double offset_y;
    double padding;
} UmiDesktopMonitorCanvas;

typedef struct UmiDesktopCanvasMonitor {
    char monitor_id[UMI_DESKTOP_ID_CAPACITY];
    char name[UMI_DESKTOP_TITLE_CAPACITY];
    UmiDesktopCanvasRect bounds;
    UmiDesktopCanvasRect work_area;
    bool primary;
    bool enabled;
} UmiDesktopCanvasMonitor;

typedef struct UmiDesktopCanvasWindow {
    char window_id[UMI_DESKTOP_ID_CAPACITY];
    char title[UMI_DESKTOP_TITLE_CAPACITY];
    char owner_application_id[UMI_DESKTOP_ID_CAPACITY];
    char context_group_id[UMI_DESKTOP_ID_CAPACITY];
    UmiDesktopCanvasRect bounds;
    UmiDesktopDockPlacement dock_placement;
    int32_t z_order;
    bool visible;
    bool selected;
    bool locked;
} UmiDesktopCanvasWindow;

UmiStatus umi_desktop_monitor_canvas_build(
    const UmiDesktopMonitorTopology *topology,
    double viewport_width,
    double viewport_height,
    double padding,
    UmiDesktopMonitorCanvas *out_canvas);
UmiStatus umi_desktop_monitor_canvas_project_monitor(
    const UmiDesktopMonitorCanvas *canvas,
    const UmiDesktopMonitor *monitor,
    UmiDesktopCanvasMonitor *out_monitor);
UmiStatus umi_desktop_monitor_canvas_project_window(
    const UmiDesktopMonitorCanvas *canvas,
    const UmiDesktopWindow *window,
    bool selected,
    bool locked,
    UmiDesktopCanvasWindow *out_window);
UmiStatus umi_desktop_monitor_canvas_delta_to_desktop(
    const UmiDesktopMonitorCanvas *canvas,
    double canvas_delta_x,
    double canvas_delta_y,
    int32_t *out_desktop_delta_x,
    int32_t *out_desktop_delta_y);
bool umi_desktop_canvas_rect_contains(
    UmiDesktopCanvasRect rect,
    double x,
    double y);

#endif
