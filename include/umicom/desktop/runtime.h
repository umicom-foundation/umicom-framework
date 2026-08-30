/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/runtime.h
 *
 * PURPOSE:
 *   Publish the public runtime contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_RUNTIME_H
#define UMICOM_DESKTOP_RUNTIME_H

#include "umicom/desktop/context_links.h"
#include "umicom/desktop/layout_catalogue.h"
#include "umicom/desktop/layout_tabs.h"
#include "umicom/desktop/monitor_topology.h"
#include "umicom/desktop/window_manager.h"

typedef struct UmiDesktopRuntime UmiDesktopRuntime;

typedef struct UmiDesktopRuntimeState {
    uint32_t structure_size;
    UmiDesktopMonitorTopology monitors;
    UmiDesktopLayoutTabs tabs;
    UmiDesktopWindowManager windows;
    UmiDesktopContextLinks context_links;
    UmiDesktopLayout active_layout;
    uint64_t revision;
} UmiDesktopRuntimeState;

UmiStatus umi_desktop_runtime_create(
    UmiApplicationContextHub *context_hub,
    UmiDesktopRuntime **out_runtime);
void umi_desktop_runtime_destroy(UmiDesktopRuntime *runtime);
UmiStatus umi_desktop_runtime_seed(UmiDesktopRuntime *runtime);
UmiStatus umi_desktop_runtime_activate_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id);
UmiStatus umi_desktop_runtime_clone_layout(
    UmiDesktopRuntime *runtime,
    const char *source_layout_id,
    const char *layout_id,
    const char *name,
    bool activate);
UmiStatus umi_desktop_runtime_replace_layout(
    UmiDesktopRuntime *runtime,
    const UmiDesktopLayout *layout,
    bool activate);
UmiStatus umi_desktop_runtime_remove_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id);
UmiStatus umi_desktop_runtime_commit_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id);
UmiStatus umi_desktop_runtime_open_window(
    UmiDesktopRuntime *runtime,
    const UmiDesktopWindow *window);
UmiStatus umi_desktop_runtime_close_window(
    UmiDesktopRuntime *runtime,
    const char *window_id);
UmiStatus umi_desktop_runtime_show_window(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    bool visible);
UmiStatus umi_desktop_runtime_place_window(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement);
UmiStatus umi_desktop_runtime_set_window_context_group(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    const char *context_group_id);
UmiStatus umi_desktop_runtime_restore_window_session(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement,
    bool visible,
    bool maximised);
UmiStatus umi_desktop_runtime_capture_state(
    const UmiDesktopRuntime *runtime,
    UmiDesktopRuntimeState *out_state);
UmiStatus umi_desktop_runtime_restore_state(
    UmiDesktopRuntime *runtime,
    const UmiDesktopRuntimeState *state);
UmiStatus umi_desktop_runtime_snapshot(
    const UmiDesktopRuntime *runtime,
    UmiDesktopSnapshot *out_snapshot);
UmiDesktopMonitorTopology *umi_desktop_runtime_monitors(
    UmiDesktopRuntime *runtime);
UmiDesktopLayoutTabs *umi_desktop_runtime_tabs(UmiDesktopRuntime *runtime);
UmiDesktopLayoutCatalogue *umi_desktop_runtime_layouts(
    UmiDesktopRuntime *runtime);
UmiDesktopWindowManager *umi_desktop_runtime_windows(
    UmiDesktopRuntime *runtime);
UmiDesktopContextLinks *umi_desktop_runtime_context_links(
    UmiDesktopRuntime *runtime);

#endif
