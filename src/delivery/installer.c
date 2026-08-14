/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/installer.c
 *
 * PURPOSE:
 *   Describe Windows and Linux installer-generation contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/installer.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_installer_plan_init(UmiInstallerPlan *plan,
                                      const char *product_name,
                                      const char *vendor,
                                      const char *version,
                                      const char *install_directory,
                                      const char *entrypoint,
                                      UmiInstallerPlatform platform,
                                      UmiInstallScope scope)
{
    UmiStatus status;
    if (plan == NULL || product_name == NULL || vendor == NULL ||
        version == NULL || install_directory == NULL || entrypoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_delivery_copy_text(plan->product_name,
                                    sizeof(plan->product_name), product_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->vendor, sizeof(plan->vendor), vendor);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->version, sizeof(plan->version), version);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->install_directory,
                                    sizeof(plan->install_directory),
                                    install_directory);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->entrypoint,
                                    sizeof(plan->entrypoint), entrypoint);
    if (status != UMI_STATUS_OK) return status;
    plan->platform = platform;
    plan->scope = scope;
    plan->create_start_menu_shortcut = platform == UMI_INSTALLER_WINDOWS;
    plan->include_uninstaller = scope != UMI_INSTALL_SCOPE_PORTABLE;
    plan->require_signature = scope != UMI_INSTALL_SCOPE_PORTABLE;
    return UMI_STATUS_OK;
}

void umi_installer_plan_set_shortcuts(UmiInstallerPlan *plan,
                                          int start_menu,
                                          int desktop)
{
    if (plan == NULL) return;
    plan->create_start_menu_shortcut = start_menu != 0;
    plan->create_desktop_shortcut = desktop != 0;
}

UmiStatus umi_installer_plan_validate(const UmiInstallerPlan *plan)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->product_name[0] == '\0' || plan->vendor[0] == '\0' ||
        plan->version[0] == '\0' || plan->install_directory[0] == '\0' ||
        plan->entrypoint[0] == '\0' ||
        (plan->platform != UMI_INSTALLER_WINDOWS &&
         plan->platform != UMI_INSTALLER_LINUX) ||
        plan->scope < UMI_INSTALL_SCOPE_USER ||
        plan->scope > UMI_INSTALL_SCOPE_PORTABLE) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (plan->scope != UMI_INSTALL_SCOPE_PORTABLE &&
        !plan->include_uninstaller) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

const char *umi_installer_plan_generator(const UmiInstallerPlan *plan)
{
    if (plan == NULL) return "unknown";
    if (plan->scope == UMI_INSTALL_SCOPE_PORTABLE) return "ZIP";
    if (plan->platform == UMI_INSTALLER_WINDOWS) return "NSIS";
    if (plan->platform == UMI_INSTALLER_LINUX) return "TGZ";
    return "unknown";
}
