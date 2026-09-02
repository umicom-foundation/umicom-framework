/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_installer_selection.c
 *
 * PURPOSE:
 *   Verify required and optional application checkboxes used by the suite
 *   installer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/installer_selection.h"

#include <assert.h>

/*
 * Exercise application and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiInstallerApplicationDefinition application(
    const char *id,
    const char *name,
    const char *component,
    const char *executable,
    uint64_t size,
    bool required)
{
    UmiInstallerApplicationDefinition definition = {0};
    definition.structure_size = sizeof(definition);
    definition.application_id = id;
    definition.display_name = name;
    definition.description = "Install this Umicom application.";
    definition.component_name = component;
    definition.executable_name = executable;
    definition.estimated_size_bytes = size;
    definition.required = required;
    definition.selected_by_default = true;
    definition.available = true;
    return definition;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiInstallerSelection selection;
    UmiInstallerSelectionSnapshot snapshot;
    UmiInstallerApplicationDefinition desk = application(
        "org.umicom.desktop", "Umicom Applications", "Desktop",
        "umicom-desk.exe", 100U, true);
    UmiInstallerApplicationDefinition studio = application(
        "org.umicom.studio", "Umicom Studio IDE", "Studio",
        "umicom-studio-ide.exe", 200U, false);
    UmiInstallerApplicationDefinition trader = application(
        "org.umicom.trader", "Umicom Trader", "Trader",
        "umicom-trader.exe", 300U, false);

    assert(umi_installer_selection_init(&selection) == UMI_STATUS_OK);
    assert(umi_installer_selection_add(&selection, &desk) == UMI_STATUS_OK);
    assert(umi_installer_selection_add(&selection, &studio) == UMI_STATUS_OK);
    assert(umi_installer_selection_add(&selection, &trader) == UMI_STATUS_OK);
    assert(umi_installer_selection_validate(&selection) == UMI_STATUS_OK);
    assert(umi_installer_selection_snapshot(
               &selection, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.application_count == 3U);
    assert(snapshot.selected_count == 3U);
    assert(snapshot.selected_size_bytes == 600U);

    assert(umi_installer_selection_set_selected(
               &selection, "org.umicom.desktop", false) ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_installer_selection_clear_optional(&selection) ==
           UMI_STATUS_OK);
    assert(umi_installer_selection_snapshot(
               &selection, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.selected_count == 1U);
    assert(snapshot.selected_size_bytes == 100U);
    assert(umi_installer_selection_select_all(&selection) == UMI_STATUS_OK);
    assert(umi_installer_selection_validate(&selection) == UMI_STATUS_OK);
    return 0;
}
