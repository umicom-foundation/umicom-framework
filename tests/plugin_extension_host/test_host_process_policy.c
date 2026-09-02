/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_process_policy.c
 *
 * PURPOSE:
 *   Exercise decide when extensions require process isolation and restart limits.
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
#include "umicom/plugin/extension_host/host_process_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostHostProcessPolicy p; umi_plugin_extension_host_host_process_policy_init(&p); /* Apply this branch only when its contract condition is satisfied. */ if(umi_plugin_extension_host_host_process_policy_isolation(&p,UMI_PLUGIN_EXTENSION_HOST_TRUST_UNKNOWN,10U)!=UMI_PLUGIN_EXTENSION_HOST_ISOLATION_RESTRICTED_PROCESS) return 1; /* Apply this branch only when its contract condition is satisfied. */ if(!umi_plugin_extension_host_host_process_policy_may_restart(&p,2U,2U)) return 2; return 0; }
