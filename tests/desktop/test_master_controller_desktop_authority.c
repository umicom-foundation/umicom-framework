/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_master_controller_desktop_authority.c
 *
 * PURPOSE:
 *   Implement the test master controller desktop authority behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Master Controller desktop authority | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>

#include "umicom/application/master_authority.h"
#include "umicom/desktop/master_authority.h"
#include "umicom/runtime/capability_registry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiMasterControllerConfig config = {0};
    UmiMasterController *master = NULL;
    UmiDesktopRuntime *desktop;
    UmiDesktopShellModel *shell;
    UmiDesktopContentRuntime *content;
    UmiDesktopSnapshot snapshot;
    config.application_name = "Desktop Authority Test";
    assert(umi_master_controller_create(&config, &master) == UMI_STATUS_OK);
    assert(umi_master_controller_install_desktop_authority(master) ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_install_application_authority(
        master, "org.umicom.studio") == UMI_STATUS_OK);
    assert(umi_master_controller_install_desktop_authority(master) ==
           UMI_STATUS_OK);
    desktop = umi_master_controller_desktop_runtime(master);
    shell = umi_master_controller_desktop_shell(master);
    content = umi_master_controller_desktop_content(master);
    assert(desktop != NULL);
    assert(shell != NULL);
    assert(content != NULL);
    assert(umi_master_controller_desktop_component_host(master) != NULL);
    assert(umi_master_controller_desktop_component_drag_drop(master) != NULL);
    assert(umi_master_controller_desktop_context_synchronizer(master) != NULL);
    assert(umi_master_controller_desktop_layout_history(master) != NULL);
    assert(umi_master_controller_desktop_session_recovery(master) != NULL);
    assert(umi_master_controller_desktop_monitor_interaction(master) != NULL);
    assert(umi_desktop_runtime_snapshot(desktop, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.layout_count == 16U && snapshot.tab_count == 16U);
    assert(snapshot.monitor_count == 1U);
    assert(strcmp(snapshot.active_layout_id, "develop") == 0);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master), "umicom.desktop") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.layouts") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.context") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.shell") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.designer") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.content") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.component-host") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.view-factories") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.component-drag-drop") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.context-synchronizer") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.layout-history") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.session-recovery") != NULL);
    assert(umi_capability_registry_find(
        umi_master_controller_capabilities(master),
        "umicom.desktop.monitor-interaction") != NULL);
    umi_master_controller_destroy(master);
    return 0;
}
