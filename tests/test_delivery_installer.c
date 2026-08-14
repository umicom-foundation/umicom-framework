/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_installer.c
 *
 * PURPOSE:
 *   Verify Windows, Linux and portable installer-generation contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/delivery/delivery.h"

int main(void)
{
    UmiInstallerPlan windows;
    UmiInstallerPlan linux;
    assert(umi_installer_plan_init(
               &windows, "Umicom Studio", "Umicom Foundation", "0.23.0",
               "Umicom Studio", "bin/umicom-studio-ide.exe",
               UMI_INSTALLER_WINDOWS, UMI_INSTALL_SCOPE_USER) ==
           UMI_STATUS_OK);
    assert(umi_installer_plan_validate(&windows) == UMI_STATUS_OK);
    assert(strcmp(umi_installer_plan_generator(&windows), "NSIS") == 0);
    assert(umi_installer_plan_init(
               &linux, "Umicom Studio", "Umicom Foundation", "0.23.0",
               "/opt/umicom-studio", "bin/umicom-studio-ide",
               UMI_INSTALLER_LINUX, UMI_INSTALL_SCOPE_PORTABLE) ==
           UMI_STATUS_OK);
    assert(strcmp(umi_installer_plan_generator(&linux), "ZIP") == 0);
    return 0;
}
