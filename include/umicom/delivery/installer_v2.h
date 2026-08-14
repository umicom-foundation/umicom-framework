/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/installer_v2.h
 *
 * PURPOSE:
 *   Describe Windows and Linux installer-generation contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_INSTALLER_V2_H
#define UMICOM_DELIVERY_INSTALLER_V2_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiInstallerPlatformV2 {
    UMI_INSTALLER_WINDOWS_V2 = 1,
    UMI_INSTALLER_LINUX_V2 = 2
} UmiInstallerPlatformV2;

typedef enum UmiInstallScopeV2 {
    UMI_INSTALL_SCOPE_USER_V2 = 1,
    UMI_INSTALL_SCOPE_MACHINE_V2 = 2,
    UMI_INSTALL_SCOPE_PORTABLE_V2 = 3
} UmiInstallScopeV2;

typedef struct UmiInstallerPlanV2 {
    char product_name[UMI_DELIVERY_ID_CAPACITY];
    char vendor[UMI_DELIVERY_ID_CAPACITY];
    char version[UMI_DELIVERY_VERSION_CAPACITY];
    char install_directory[UMI_DELIVERY_PATH_CAPACITY];
    char entrypoint[UMI_DELIVERY_PATH_CAPACITY];
    UmiInstallerPlatformV2 platform;
    UmiInstallScopeV2 scope;
    int create_start_menu_shortcut;
    int create_desktop_shortcut;
    int include_uninstaller;
    int require_signature;
} UmiInstallerPlanV2;

UmiStatus umi_installer_plan_v2_init(UmiInstallerPlanV2 *plan,
                                      const char *product_name,
                                      const char *vendor,
                                      const char *version,
                                      const char *install_directory,
                                      const char *entrypoint,
                                      UmiInstallerPlatformV2 platform,
                                      UmiInstallScopeV2 scope);
void umi_installer_plan_v2_set_shortcuts(UmiInstallerPlanV2 *plan,
                                          int start_menu,
                                          int desktop);
UmiStatus umi_installer_plan_v2_validate(const UmiInstallerPlanV2 *plan);
const char *umi_installer_plan_v2_generator(const UmiInstallerPlanV2 *plan);

#ifdef __cplusplus
}
#endif
#endif
