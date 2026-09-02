/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_recovery_plan.c
 *
 * PURPOSE:
 *   Exercise represent a reviewable extension recovery plan before restart or reactivation.
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
#include "umicom/plugin/extension_host/recovery_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostRecoveryPlan p; umi_plugin_extension_host_recovery_plan_init(&p); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_recovery_plan_add(&p,"verify",20U,1)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_recovery_plan_ready(&p)) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_recovery_plan_complete(&p,"verify")!=UMI_STATUS_OK) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_plugin_extension_host_recovery_plan_ready(&p)||umi_plugin_extension_host_recovery_plan_maximum_risk(&p)!=20U) return 4; return 0; }
