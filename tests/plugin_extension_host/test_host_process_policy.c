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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_process_policy.h"
int main(void) { UmiPluginExtensionHostHostProcessPolicy p; umi_plugin_extension_host_host_process_policy_init(&p); if(umi_plugin_extension_host_host_process_policy_isolation(&p,UMI_PLUGIN_EXTENSION_HOST_TRUST_UNKNOWN,10U)!=UMI_PLUGIN_EXTENSION_HOST_ISOLATION_RESTRICTED_PROCESS) return 1; if(!umi_plugin_extension_host_host_process_policy_may_restart(&p,2U,2U)) return 2; return 0; }
