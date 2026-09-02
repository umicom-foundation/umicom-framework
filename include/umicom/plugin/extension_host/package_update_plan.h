/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/package_update_plan.h
 *
 * PURPOSE:
 *   Represent a reviewable extension update plan with rollback checkpoints.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_UPDATE_PLAN_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_UPDATE_PLAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host package update plan step data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostPackageUpdatePlanStep { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t risk; int required; int completed; } UmiPluginExtensionHostPackageUpdatePlanStep;
/**
 * Represent the plugin extension host package update plan data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostPackageUpdatePlan { UmiPluginExtensionHostPackageUpdatePlanStep steps[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPackageUpdatePlan;

/**
 * Initialise plugin extension host package update plan from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_package_update_plan_init(UmiPluginExtensionHostPackageUpdatePlan *plan);
/**
 * Add plugin extension host package update plan only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_plugin_extension_host_package_update_plan_add(UmiPluginExtensionHostPackageUpdatePlan *plan, const char *id, uint32_t risk, int required);
/**
 * Provide the plugin extension host package update plan complete operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_package_update_plan_complete(UmiPluginExtensionHostPackageUpdatePlan *plan, const char *id);
/**
 * Provide the plugin extension host package update plan ready operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_package_update_plan_ready(const UmiPluginExtensionHostPackageUpdatePlan *plan);
/**
 * Provide the plugin extension host package update plan maximum risk operation used by
 * this module and its client applications.
 */
uint32_t umi_plugin_extension_host_package_update_plan_maximum_risk(const UmiPluginExtensionHostPackageUpdatePlan *plan);

#ifdef __cplusplus
}
#endif

#endif
