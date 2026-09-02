/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/recovery_plan.h
 *
 * PURPOSE:
 *   Represent a reviewable extension recovery plan before restart or reactivation.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_RECOVERY_PLAN_H
#define UMICOM_PLUGIN_EXTENSION_HOST_RECOVERY_PLAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host recovery plan step data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostRecoveryPlanStep { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t risk; int required; int completed; } UmiPluginExtensionHostRecoveryPlanStep;
/**
 * Represent the plugin extension host recovery plan data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostRecoveryPlan { UmiPluginExtensionHostRecoveryPlanStep steps[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostRecoveryPlan;

/**
 * Initialise plugin extension host recovery plan from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_recovery_plan_init(UmiPluginExtensionHostRecoveryPlan *plan);
/**
 * Add plugin extension host recovery plan only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_plugin_extension_host_recovery_plan_add(UmiPluginExtensionHostRecoveryPlan *plan, const char *id, uint32_t risk, int required);
/**
 * Provide the plugin extension host recovery plan complete operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_recovery_plan_complete(UmiPluginExtensionHostRecoveryPlan *plan, const char *id);
/**
 * Provide the plugin extension host recovery plan ready operation used by this module and
 * its client applications.
 */
int umi_plugin_extension_host_recovery_plan_ready(const UmiPluginExtensionHostRecoveryPlan *plan);
/**
 * Provide the plugin extension host recovery plan maximum risk operation used by this
 * module and its client applications.
 */
uint32_t umi_plugin_extension_host_recovery_plan_maximum_risk(const UmiPluginExtensionHostRecoveryPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
