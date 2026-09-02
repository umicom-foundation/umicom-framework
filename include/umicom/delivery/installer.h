/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/installer.h
 *
 * PURPOSE:
 *   Describe Windows and Linux installer-generation contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_INSTALLER_H
#define UMICOM_DELIVERY_INSTALLER_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named installer platform values accepted by this public contract.
 */
typedef enum UmiInstallerPlatform {
    UMI_INSTALLER_WINDOWS = 1,
    UMI_INSTALLER_LINUX = 2
} UmiInstallerPlatform;

/**
 * List the named install scope values accepted by this public contract.
 */
typedef enum UmiInstallScope {
    UMI_INSTALL_SCOPE_USER = 1,
    UMI_INSTALL_SCOPE_MACHINE = 2,
    UMI_INSTALL_SCOPE_PORTABLE = 3
} UmiInstallScope;

/**
 * Represent the installer plan data shared with callers of this public contract.
 */
typedef struct UmiInstallerPlan {
    char product_name[UMI_DELIVERY_ID_CAPACITY];
    char vendor[UMI_DELIVERY_ID_CAPACITY];
    char version[UMI_DELIVERY_VERSION_CAPACITY];
    char install_directory[UMI_DELIVERY_PATH_CAPACITY];
    char entrypoint[UMI_DELIVERY_PATH_CAPACITY];
    UmiInstallerPlatform platform;
    UmiInstallScope scope;
    int create_start_menu_shortcut;
    int create_desktop_shortcut;
    int include_uninstaller;
    int require_signature;
} UmiInstallerPlan;

/**
 * Initialise installer plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_installer_plan_init(UmiInstallerPlan *plan,
                                      const char *product_name,
                                      const char *vendor,
                                      const char *version,
                                      const char *install_directory,
                                      const char *entrypoint,
                                      UmiInstallerPlatform platform,
                                      UmiInstallScope scope);
/**
 * Provide the installer plan set shortcuts operation used by this module and its client
 * applications.
 */
void umi_installer_plan_set_shortcuts(UmiInstallerPlan *plan,
                                          int start_menu,
                                          int desktop);
/**
 * Check that installer plan satisfies its contract before another service relies on it.
 */
UmiStatus umi_installer_plan_validate(const UmiInstallerPlan *plan);
/**
 * Provide the installer plan generator operation used by this module and its client
 * applications.
 */
const char *umi_installer_plan_generator(const UmiInstallerPlan *plan);

#ifdef __cplusplus
}
#endif
#endif
