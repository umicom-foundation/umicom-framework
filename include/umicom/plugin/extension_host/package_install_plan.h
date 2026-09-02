/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/package_install_plan.h
 *
 * PURPOSE:
 *   Represent a reviewable extension installation plan before filesystem mutation.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_INSTALL_PLAN_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_INSTALL_PLAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host package install plan step data shared with callers
 * of this public contract.
 */
typedef struct UmiPluginExtensionHostPackageInstallPlanStep { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t risk; int required; int completed; } UmiPluginExtensionHostPackageInstallPlanStep;
/**
 * Represent the plugin extension host package install plan data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostPackageInstallPlan { UmiPluginExtensionHostPackageInstallPlanStep steps[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPackageInstallPlan;

/**
 * Initialise plugin extension host package install plan from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_package_install_plan_init(UmiPluginExtensionHostPackageInstallPlan *plan);
/**
 * Add plugin extension host package install plan only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_plugin_extension_host_package_install_plan_add(UmiPluginExtensionHostPackageInstallPlan *plan, const char *id, uint32_t risk, int required);
/**
 * Provide the plugin extension host package install plan complete operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_package_install_plan_complete(UmiPluginExtensionHostPackageInstallPlan *plan, const char *id);
/**
 * Provide the plugin extension host package install plan ready operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_package_install_plan_ready(const UmiPluginExtensionHostPackageInstallPlan *plan);
/**
 * Provide the plugin extension host package install plan maximum risk operation used by
 * this module and its client applications.
 */
uint32_t umi_plugin_extension_host_package_install_plan_maximum_risk(const UmiPluginExtensionHostPackageInstallPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
