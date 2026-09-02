/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/installer.c
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
#include "umicom/delivery/installer.h"
#include "delivery_internal.h"
#include <string.h>

/*
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
                                      UmiInstallScope scope)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || product_name == NULL || vendor == NULL ||
        version == NULL || install_directory == NULL || entrypoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_delivery_copy_text(plan->product_name,
                                    sizeof(plan->product_name), product_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->vendor, sizeof(plan->vendor), vendor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->version, sizeof(plan->version), version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->install_directory,
                                    sizeof(plan->install_directory),
                                    install_directory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->entrypoint,
                                    sizeof(plan->entrypoint), entrypoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    plan->platform = platform;
    plan->scope = scope;
    plan->create_start_menu_shortcut = platform == UMI_INSTALLER_WINDOWS;
    plan->include_uninstaller = scope != UMI_INSTALL_SCOPE_PORTABLE;
    plan->require_signature = scope != UMI_INSTALL_SCOPE_PORTABLE;
    return UMI_STATUS_OK;
}

/*
 * Provide the installer plan set shortcuts operation used by this module and its client
 * applications.
 */
void umi_installer_plan_set_shortcuts(UmiInstallerPlan *plan,
                                          int start_menu,
                                          int desktop)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    plan->create_start_menu_shortcut = start_menu != 0;
    plan->create_desktop_shortcut = desktop != 0;
}

/* Check that installer plan satisfies its contract before another service relies on it. */
UmiStatus umi_installer_plan_validate(const UmiInstallerPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->product_name[0] == '\0' || plan->vendor[0] == '\0' ||
        plan->version[0] == '\0' || plan->install_directory[0] == '\0' ||
        plan->entrypoint[0] == '\0' ||
        (plan->platform != UMI_INSTALLER_WINDOWS &&
         plan->platform != UMI_INSTALLER_LINUX) ||
        plan->scope < UMI_INSTALL_SCOPE_USER ||
        plan->scope > UMI_INSTALL_SCOPE_PORTABLE) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->scope != UMI_INSTALL_SCOPE_PORTABLE &&
        !plan->include_uninstaller) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the installer plan generator operation used by this module and its client
 * applications.
 */
const char *umi_installer_plan_generator(const UmiInstallerPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return "unknown";
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->scope == UMI_INSTALL_SCOPE_PORTABLE) return "ZIP";
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->platform == UMI_INSTALLER_WINDOWS) return "NSIS";
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->platform == UMI_INSTALLER_LINUX) return "TGZ";
    return "unknown";
}
