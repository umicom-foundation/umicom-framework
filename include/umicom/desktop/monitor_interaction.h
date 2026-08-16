/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/monitor_interaction.h
 *
 * PURPOSE:
 *   Define reusable multi-monitor focus, movement, affinity and reconciliation
 *   services. All application windows use the same Framework geometry policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_MONITOR_INTERACTION_H
#define UMICOM_DESKTOP_MONITOR_INTERACTION_H

#include "umicom/desktop/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_MONITOR_AFFINITY_MAX UMI_DESKTOP_MAX_WINDOWS

typedef struct UmiDesktopMonitorAffinity {
    uint32_t structure_size;
    char window_id[UMI_DESKTOP_ID_CAPACITY];
    char monitor_id[UMI_DESKTOP_ID_CAPACITY];
    UmiDesktopRect bounds;
    UmiDesktopDockPlacement placement;
    uint64_t revision;
} UmiDesktopMonitorAffinity;

typedef struct UmiDesktopMonitorInteractionSnapshot {
    uint32_t structure_size;
    char focused_monitor_id[UMI_DESKTOP_ID_CAPACITY];
    size_t enabled_monitor_count;
    size_t affinity_count;
    size_t relocated_window_count;
    uint64_t revision;
} UmiDesktopMonitorInteractionSnapshot;

typedef struct UmiDesktopMonitorInteraction UmiDesktopMonitorInteraction;

UmiStatus umi_desktop_monitor_interaction_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopMonitorInteraction **out_interaction);
void umi_desktop_monitor_interaction_destroy(
    UmiDesktopMonitorInteraction *interaction);
UmiStatus umi_desktop_monitor_interaction_focus(
    UmiDesktopMonitorInteraction *interaction,
    const char *monitor_id);
UmiStatus umi_desktop_monitor_interaction_capture_affinities(
    UmiDesktopMonitorInteraction *interaction);
UmiStatus umi_desktop_monitor_interaction_move_window(
    UmiDesktopMonitorInteraction *interaction,
    const char *window_id,
    const char *monitor_id);
UmiStatus umi_desktop_monitor_interaction_move_window_next(
    UmiDesktopMonitorInteraction *interaction,
    const char *window_id);
UmiStatus umi_desktop_monitor_interaction_restore_affinity(
    UmiDesktopMonitorInteraction *interaction,
    const char *window_id);
UmiStatus umi_desktop_monitor_interaction_reconcile(
    UmiDesktopMonitorInteraction *interaction);
UmiStatus umi_desktop_monitor_interaction_affinity_at(
    const UmiDesktopMonitorInteraction *interaction,
    size_t index,
    UmiDesktopMonitorAffinity *out_affinity);
UmiStatus umi_desktop_monitor_interaction_snapshot(
    const UmiDesktopMonitorInteraction *interaction,
    UmiDesktopMonitorInteractionSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
