/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_recovery_policy.c
 *
 * PURPOSE:
 *   Exercise define bounded restart, backoff and checkpoint requirements after extension failure.
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
#include "umicom/plugin/extension_host/recovery_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostRecoveryPolicy p; umi_plugin_extension_host_recovery_policy_init(&p); /* Apply this branch only when its contract condition is satisfied. */ if(umi_plugin_extension_host_recovery_policy_evaluate(&p,1U,1,1)!=UMI_PLUGIN_EXTENSION_HOST_ALLOW) return 1; /* Apply this branch only when its contract condition is satisfied. */ if(umi_plugin_extension_host_recovery_policy_backoff_ms(&p,2U)!=2000U) return 2; return 0; }
