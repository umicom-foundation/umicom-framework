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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the plugin extension host package activation plan step data shared with
 * callers of this public contract.
 */
typedef struct UmiPluginExtensionHostPackageActivationPlanStep { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t risk; int required; int completed; } UmiPluginExtensionHostPackageActivationPlanStep;
/**
 * Represent the plugin extension host package activation plan data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostPackageActivationPlan { UmiPluginExtensionHostPackageActivationPlanStep steps[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPackageActivationPlan;

/**
 * Initialise plugin extension host package activation plan from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_package_activation_plan_init(UmiPluginExtensionHostPackageActivationPlan *plan);
/**
 * Add plugin extension host package activation plan only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_plugin_extension_host_package_activation_plan_add(UmiPluginExtensionHostPackageActivationPlan *plan, const char *id, uint32_t risk, int required);
/**
 * Provide the plugin extension host package activation plan complete operation used by
 * this module and its client applications.
 */
UmiStatus umi_plugin_extension_host_package_activation_plan_complete(UmiPluginExtensionHostPackageActivationPlan *plan, const char *id);
/**
 * Provide the plugin extension host package activation plan ready operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_package_activation_plan_ready(const UmiPluginExtensionHostPackageActivationPlan *plan);
/**
 * Provide the plugin extension host package activation plan maximum risk operation used by
 * this module and its client applications.
 */
uint32_t umi_plugin_extension_host_package_activation_plan_maximum_risk(const UmiPluginExtensionHostPackageActivationPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
