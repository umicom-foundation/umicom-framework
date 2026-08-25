/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/package_activation_plan.h
 *
 * PURPOSE:
 *   Represent activation stages after verification, dependency and permission checks.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_ACTIVATION_PLAN_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_ACTIVATION_PLAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostPackageActivationPlanStep { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t risk; int required; int completed; } UmiPluginExtensionHostPackageActivationPlanStep;
typedef struct UmiPluginExtensionHostPackageActivationPlan { UmiPluginExtensionHostPackageActivationPlanStep steps[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPackageActivationPlan;

void umi_plugin_extension_host_package_activation_plan_init(UmiPluginExtensionHostPackageActivationPlan *plan);
UmiStatus umi_plugin_extension_host_package_activation_plan_add(UmiPluginExtensionHostPackageActivationPlan *plan, const char *id, uint32_t risk, int required);
UmiStatus umi_plugin_extension_host_package_activation_plan_complete(UmiPluginExtensionHostPackageActivationPlan *plan, const char *id);
int umi_plugin_extension_host_package_activation_plan_ready(const UmiPluginExtensionHostPackageActivationPlan *plan);
uint32_t umi_plugin_extension_host_package_activation_plan_maximum_risk(const UmiPluginExtensionHostPackageActivationPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
