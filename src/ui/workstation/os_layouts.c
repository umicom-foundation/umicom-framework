/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/os_layouts.c
 *
 * PURPOSE:
 *   Implement Umicom OS desktop, systems-development and system-monitor workstation layout presets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/os_layouts.h"

/* Provide the add operation used by this module and its client applications. */
static UmiStatus add(UmiWsLayoutTemplate *layout, const char *id, UmiWsDockRegion region, int32_t row, int32_t col, double weight) {
    return umi_ws_layout_template_add(layout, id, region, row, col, weight);
}

/*
 * Provide the ws os layout desktop operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_os_layout_desktop(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "os.desktop", "Umicom OS Desktop", UMI_WS_DOMAIN_OS);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "os.application-rail", UMI_WS_DOCK_LEFT, 0, 0, 0.35)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "os.desktop-canvas", UMI_WS_DOCK_CENTRE, 0, 1, 2.5)) != UMI_STATUS_OK) return s;
    return add(layout, "os.notification-centre", UMI_WS_DOCK_RIGHT, 0, 2, 0.6);
}

/*
 * Provide the ws os layout systems operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_os_layout_systems(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "os.systems-development", "Systems Development", UMI_WS_DOMAIN_OS);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "os.source-tree", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "os.editor", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "os.machine-state", UMI_WS_DOCK_RIGHT, 0, 2, 0.8)) != UMI_STATUS_OK) return s;
    return add(layout, "os.console", UMI_WS_DOCK_BOTTOM, 1, 1, 0.7);
}

/*
 * Provide the ws os layout monitor operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_os_layout_monitor(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "os.monitor", "System Monitor", UMI_WS_DOMAIN_OS);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "os.processes", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "os.performance-charts", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "os.services", UMI_WS_DOCK_RIGHT, 0, 2, 0.8)) != UMI_STATUS_OK) return s;
    return add(layout, "os.logs", UMI_WS_DOCK_BOTTOM, 1, 1, 0.8);
}
