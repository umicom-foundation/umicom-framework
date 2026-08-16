/* Umicom Framework Tests | Master Controller desktop authority | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>

#include "umicom/application/master_authority.h"
#include "umicom/desktop/master_authority.h"
#include "umicom/runtime/capability_registry.h"

int main(void)
{
    UmiMasterControllerConfig config = {0};
    UmiMasterController *master = NULL;
    UmiDesktopRuntime *desktop;
    UmiDesktopShellModel *shell;
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
    assert(desktop != NULL);
    assert(shell != NULL);
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
    umi_master_controller_destroy(master);
    return 0;
}
