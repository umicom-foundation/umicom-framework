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

/**
 * Represent the desktop runtime data shared with callers of this public contract.
 */
typedef struct UmiDesktopRuntime UmiDesktopRuntime;

/**
 * Represent the desktop runtime state data shared with callers of this public contract.
 */
typedef struct UmiDesktopRuntimeState {
    uint32_t structure_size;
    UmiDesktopMonitorTopology monitors;
    UmiDesktopLayoutTabs tabs;
    UmiDesktopWindowManager windows;
    UmiDesktopContextLinks context_links;
    UmiDesktopLayout active_layout;
    uint64_t revision;
} UmiDesktopRuntimeState;

/**
 * Initialise desktop runtime from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_desktop_runtime_create(
    UmiApplicationContextHub *context_hub,
    UmiDesktopRuntime **out_runtime);
/**
 * Release or reset state held by desktop runtime so the same storage can be reused safely.
 */
void umi_desktop_runtime_destroy(UmiDesktopRuntime *runtime);
/**
 * Provide the desktop runtime seed operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_seed(UmiDesktopRuntime *runtime);
/**
 * Provide the desktop runtime activate layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_activate_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id);
/**
 * Provide the desktop runtime clone layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_clone_layout(
    UmiDesktopRuntime *runtime,
    const char *source_layout_id,
    const char *layout_id,
    const char *name,
    bool activate);
/**
 * Provide the desktop runtime replace layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_replace_layout(
    UmiDesktopRuntime *runtime,
    const UmiDesktopLayout *layout,
    bool activate);
/**
 * Provide the desktop runtime remove layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_remove_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id);
/**
 * Provide the desktop runtime commit layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_commit_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id);
/**
 * Provide the desktop runtime open window operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_open_window(
    UmiDesktopRuntime *runtime,
    const UmiDesktopWindow *window);
/**
 * Provide the desktop runtime close window operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_close_window(
    UmiDesktopRuntime *runtime,
    const char *window_id);
/**
 * Provide the desktop runtime show window operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_show_window(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    bool visible);
/**
 * Provide the desktop runtime place window operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_place_window(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement);
/**
 * Provide the desktop runtime set window context group operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_runtime_set_window_context_group(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    const char *context_group_id);
/**
 * Provide the desktop runtime restore window session operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_runtime_restore_window_session(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement,
    bool visible,
    bool maximised);
/**
 * Provide the desktop runtime capture state operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_capture_state(
    const UmiDesktopRuntime *runtime,
    UmiDesktopRuntimeState *out_state);
/**
 * Provide the desktop runtime restore state operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_restore_state(
    UmiDesktopRuntime *runtime,
    const UmiDesktopRuntimeState *state);
/**
 * Provide the desktop runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_snapshot(
    const UmiDesktopRuntime *runtime,
    UmiDesktopSnapshot *out_snapshot);
/**
 * Provide the desktop runtime monitors operation used by this module and its client
 * applications.
 */
UmiDesktopMonitorTopology *umi_desktop_runtime_monitors(
    UmiDesktopRuntime *runtime);
/**
 * Provide the desktop runtime tabs operation used by this module and its client
 * applications.
 */
UmiDesktopLayoutTabs *umi_desktop_runtime_tabs(UmiDesktopRuntime *runtime);
/**
 * Provide the desktop runtime layouts operation used by this module and its client
 * applications.
 */
UmiDesktopLayoutCatalogue *umi_desktop_runtime_layouts(
    UmiDesktopRuntime *runtime);
/**
 * Provide the desktop runtime windows operation used by this module and its client
 * applications.
 */
UmiDesktopWindowManager *umi_desktop_runtime_windows(
    UmiDesktopRuntime *runtime);
/**
 * Provide the desktop runtime context links operation used by this module and its client
 * applications.
 */
UmiDesktopContextLinks *umi_desktop_runtime_context_links(
    UmiDesktopRuntime *runtime);

#endif
