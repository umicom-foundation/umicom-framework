/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/package_remove_plan.h
 *
 * PURPOSE:
 *   Represent a reviewable extension removal plan that preserves recovery evidence.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_REMOVE_PLAN_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_REMOVE_PLAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostPackageRemovePlanStep { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t risk; int required; int completed; } UmiPluginExtensionHostPackageRemovePlanStep;
typedef struct UmiPluginExtensionHostPackageRemovePlan { UmiPluginExtensionHostPackageRemovePlanStep steps[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPackageRemovePlan;

void umi_plugin_extension_host_package_remove_plan_init(UmiPluginExtensionHostPackageRemovePlan *plan);
UmiStatus umi_plugin_extension_host_package_remove_plan_add(UmiPluginExtensionHostPackageRemovePlan *plan, const char *id, uint32_t risk, int required);
UmiStatus umi_plugin_extension_host_package_remove_plan_complete(UmiPluginExtensionHostPackageRemovePlan *plan, const char *id);
int umi_plugin_extension_host_package_remove_plan_ready(const UmiPluginExtensionHostPackageRemovePlan *plan);
uint32_t umi_plugin_extension_host_package_remove_plan_maximum_risk(const UmiPluginExtensionHostPackageRemovePlan *plan);

#ifdef __cplusplus
}
#endif

#endif
