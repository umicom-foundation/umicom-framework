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

/**
 * Represent the plugin extension host package remove plan step data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostPackageRemovePlanStep { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t risk; int required; int completed; } UmiPluginExtensionHostPackageRemovePlanStep;
/**
 * Represent the plugin extension host package remove plan data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostPackageRemovePlan { UmiPluginExtensionHostPackageRemovePlanStep steps[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPackageRemovePlan;

/**
 * Initialise plugin extension host package remove plan from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_package_remove_plan_init(UmiPluginExtensionHostPackageRemovePlan *plan);
/**
 * Add plugin extension host package remove plan only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_plugin_extension_host_package_remove_plan_add(UmiPluginExtensionHostPackageRemovePlan *plan, const char *id, uint32_t risk, int required);
/**
 * Provide the plugin extension host package remove plan complete operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_package_remove_plan_complete(UmiPluginExtensionHostPackageRemovePlan *plan, const char *id);
/**
 * Provide the plugin extension host package remove plan ready operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_package_remove_plan_ready(const UmiPluginExtensionHostPackageRemovePlan *plan);
/**
 * Provide the plugin extension host package remove plan maximum risk operation used by
 * this module and its client applications.
 */
uint32_t umi_plugin_extension_host_package_remove_plan_maximum_risk(const UmiPluginExtensionHostPackageRemovePlan *plan);

#ifdef __cplusplus
}
#endif

#endif
