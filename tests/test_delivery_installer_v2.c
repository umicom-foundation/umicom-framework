/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_installer_v2.c
 *
 * PURPOSE:
 *   Verify Windows, Linux and portable installer-generation contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/delivery/delivery.h"

int main(void)
{
    UmiInstallerPlanV2 windows;
    UmiInstallerPlanV2 linux;
    assert(umi_installer_plan_v2_init(
               &windows, "Umicom Studio", "Umicom Foundation", "0.23.0",
               "Umicom Studio", "bin/umicom-studio-ide.exe",
               UMI_INSTALLER_WINDOWS_V2, UMI_INSTALL_SCOPE_USER_V2) ==
           UMI_STATUS_OK);
    assert(umi_installer_plan_v2_validate(&windows) == UMI_STATUS_OK);
    assert(strcmp(umi_installer_plan_v2_generator(&windows), "NSIS") == 0);
    assert(umi_installer_plan_v2_init(
               &linux, "Umicom Studio", "Umicom Foundation", "0.23.0",
               "/opt/umicom-studio", "bin/umicom-studio-ide",
               UMI_INSTALLER_LINUX_V2, UMI_INSTALL_SCOPE_PORTABLE_V2) ==
           UMI_STATUS_OK);
    assert(strcmp(umi_installer_plan_v2_generator(&linux), "ZIP") == 0);
    return 0;
}
