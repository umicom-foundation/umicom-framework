/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_monitor_interaction.c
 *
 * PURPOSE:
 *   Implement the test monitor interaction behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Desktop monitor interaction | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/desktop/monitor_interaction.h"

int main(void)
{
    UmiApplicationContextHub *hub = NULL;
    UmiDesktopRuntime *runtime = NULL;
    UmiDesktopMonitorInteraction *interaction = NULL;
    UmiDesktopMonitor secondary = {0};
    UmiDesktopMonitorInteractionSnapshot snapshot;
    const UmiDesktopWindow *window;
    assert(umi_application_context_hub_create(&hub) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(hub, &runtime) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(runtime) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_clone_layout(
        runtime, "develop", "monitor-test", "Monitor Test", true) ==
        UMI_STATUS_OK);
    (void)snprintf(secondary.monitor_id, sizeof(secondary.monitor_id), "%s",
                   "secondary");
    (void)snprintf(secondary.name, sizeof(secondary.name), "%s",
                   "Secondary Monitor");
    secondary.bounds = (UmiDesktopRect){1920, 0, 2560, 1440};
    secondary.work_area = (UmiDesktopRect){1920, 0, 2560, 1400};
    secondary.scale = 1.25;
    secondary.refresh_rate_hz = 120.0;
    secondary.enabled = true;
    assert(umi_desktop_monitor_topology_add(
        umi_desktop_runtime_monitors(runtime), &secondary) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_interaction_create(
        runtime, &interaction) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_interaction_capture_affinities(interaction) ==
           UMI_STATUS_OK);
    assert(umi_desktop_monitor_interaction_move_window_next(
        interaction, "project-explorer") == UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "project-explorer");
    assert(window != NULL && strcmp(window->monitor_id, "secondary") == 0);
    secondary.enabled = false;
    assert(umi_desktop_monitor_topology_update(
        umi_desktop_runtime_monitors(runtime), &secondary) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_interaction_reconcile(interaction) ==
           UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "project-explorer");
    assert(window != NULL && strcmp(window->monitor_id, "primary") == 0);
    assert(umi_desktop_monitor_interaction_restore_affinity(
        interaction, "project-explorer") == UMI_STATUS_UNAVAILABLE);
    secondary.enabled = true;
    assert(umi_desktop_monitor_topology_update(
        umi_desktop_runtime_monitors(runtime), &secondary) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_interaction_restore_affinity(
        interaction, "project-explorer") == UMI_STATUS_OK);
    assert(umi_desktop_monitor_interaction_snapshot(
        interaction, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.enabled_monitor_count == 2U);
    assert(snapshot.relocated_window_count >= 1U);
    umi_desktop_monitor_interaction_destroy(interaction);
    umi_desktop_runtime_destroy(runtime);
    umi_application_context_hub_destroy(hub);
    return 0;
}
