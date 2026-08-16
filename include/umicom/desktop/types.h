/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/types.h
 *
 * PURPOSE:
 *   Define stable, toolkit-neutral contracts for the federated Umicom desktop.
 *   Every application contributes Framework component identifiers; the Master
 *   Controller owns placement, visibility, context links and persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_TYPES_H
#define UMICOM_DESKTOP_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UMI_DESKTOP_ID_CAPACITY 128U
#define UMI_DESKTOP_TITLE_CAPACITY 192U
#define UMI_DESKTOP_DESCRIPTION_CAPACITY 384U
#define UMI_DESKTOP_MAX_MONITORS 16U
#define UMI_DESKTOP_MAX_TABS 32U
#define UMI_DESKTOP_MAX_WINDOWS 128U
#define UMI_DESKTOP_MAX_LAYOUTS 32U
#define UMI_DESKTOP_MAX_LAYOUT_WINDOWS 32U

typedef enum UmiDesktopDockPlacement {
    UMI_DESKTOP_DOCK_CANVAS = 1,
    UMI_DESKTOP_DOCK_DOCUMENT,
    UMI_DESKTOP_DOCK_LEFT,
    UMI_DESKTOP_DOCK_RIGHT,
    UMI_DESKTOP_DOCK_TOP,
    UMI_DESKTOP_DOCK_BOTTOM,
    UMI_DESKTOP_DOCK_FLOATING
} UmiDesktopDockPlacement;

typedef enum UmiDesktopLayoutTabPlacement {
    UMI_DESKTOP_LAYOUT_TABS_BOTTOM = 1
} UmiDesktopLayoutTabPlacement;

typedef struct UmiDesktopRect {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} UmiDesktopRect;

typedef struct UmiDesktopMonitor {
    char monitor_id[UMI_DESKTOP_ID_CAPACITY];
    char name[UMI_DESKTOP_TITLE_CAPACITY];
    UmiDesktopRect bounds;
    UmiDesktopRect work_area;
    double scale;
    double refresh_rate_hz;
    bool primary;
    bool enabled;
} UmiDesktopMonitor;

typedef struct UmiDesktopWindow {
    char window_id[UMI_DESKTOP_ID_CAPACITY];
    char title[UMI_DESKTOP_TITLE_CAPACITY];
    char component_id[UMI_DESKTOP_ID_CAPACITY];
    char owner_application_id[UMI_DESKTOP_ID_CAPACITY];
    char monitor_id[UMI_DESKTOP_ID_CAPACITY];
    char context_group_id[UMI_DESKTOP_ID_CAPACITY];
    UmiDesktopRect bounds;
    UmiDesktopDockPlacement dock_placement;
    int32_t z_order;
    bool visible;
    bool maximised;
    bool closable;
    bool resizable;
} UmiDesktopWindow;

typedef struct UmiDesktopLayoutTab {
    char tab_id[UMI_DESKTOP_ID_CAPACITY];
    char layout_id[UMI_DESKTOP_ID_CAPACITY];
    char label[UMI_DESKTOP_TITLE_CAPACITY];
    uint32_t order;
    bool active;
    bool pinned;
    bool dirty;
    bool closable;
} UmiDesktopLayoutTab;

typedef struct UmiDesktopLayout {
    char layout_id[UMI_DESKTOP_ID_CAPACITY];
    char name[UMI_DESKTOP_TITLE_CAPACITY];
    char category[UMI_DESKTOP_ID_CAPACITY];
    char description[UMI_DESKTOP_DESCRIPTION_CAPACITY];
    UmiDesktopWindow windows[UMI_DESKTOP_MAX_LAYOUT_WINDOWS];
    size_t window_count;
    bool built_in;
    bool locked;
    uint64_t revision;
} UmiDesktopLayout;

typedef struct UmiDesktopSnapshot {
    size_t monitor_count;
    size_t layout_count;
    size_t tab_count;
    size_t window_count;
    size_t context_group_count;
    char active_layout_id[UMI_DESKTOP_ID_CAPACITY];
    char primary_monitor_id[UMI_DESKTOP_ID_CAPACITY];
    UmiDesktopLayoutTabPlacement tab_placement;
    uint64_t revision;
} UmiDesktopSnapshot;

#endif
