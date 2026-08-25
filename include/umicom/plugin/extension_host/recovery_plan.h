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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiPluginExtensionHostRecoveryPlanStep { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t risk; int required; int completed; } UmiPluginExtensionHostRecoveryPlanStep;
typedef struct UmiPluginExtensionHostRecoveryPlan { UmiPluginExtensionHostRecoveryPlanStep steps[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostRecoveryPlan;

void umi_plugin_extension_host_recovery_plan_init(UmiPluginExtensionHostRecoveryPlan *plan);
UmiStatus umi_plugin_extension_host_recovery_plan_add(UmiPluginExtensionHostRecoveryPlan *plan, const char *id, uint32_t risk, int required);
UmiStatus umi_plugin_extension_host_recovery_plan_complete(UmiPluginExtensionHostRecoveryPlan *plan, const char *id);
int umi_plugin_extension_host_recovery_plan_ready(const UmiPluginExtensionHostRecoveryPlan *plan);
uint32_t umi_plugin_extension_host_recovery_plan_maximum_risk(const UmiPluginExtensionHostRecoveryPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
