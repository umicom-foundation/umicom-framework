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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiPluginExtensionHostPackageInstallPlanStep { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t risk; int required; int completed; } UmiPluginExtensionHostPackageInstallPlanStep;
typedef struct UmiPluginExtensionHostPackageInstallPlan { UmiPluginExtensionHostPackageInstallPlanStep steps[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPackageInstallPlan;

void umi_plugin_extension_host_package_install_plan_init(UmiPluginExtensionHostPackageInstallPlan *plan);
UmiStatus umi_plugin_extension_host_package_install_plan_add(UmiPluginExtensionHostPackageInstallPlan *plan, const char *id, uint32_t risk, int required);
UmiStatus umi_plugin_extension_host_package_install_plan_complete(UmiPluginExtensionHostPackageInstallPlan *plan, const char *id);
int umi_plugin_extension_host_package_install_plan_ready(const UmiPluginExtensionHostPackageInstallPlan *plan);
uint32_t umi_plugin_extension_host_package_install_plan_maximum_risk(const UmiPluginExtensionHostPackageInstallPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
