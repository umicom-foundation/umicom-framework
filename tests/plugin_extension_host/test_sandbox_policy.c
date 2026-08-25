/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_sandbox_policy.c
 *
 * PURPOSE:
 *   Exercise evaluate network, process, workspace-write and external-path sandbox capabilities.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/sandbox_policy.h"
int main(void) { UmiPluginExtensionHostSandboxPolicy p; umi_plugin_extension_host_sandbox_policy_init(&p); if(umi_plugin_extension_host_sandbox_policy_evaluate(&p,0,0,0,0,0)!=UMI_PLUGIN_EXTENSION_HOST_ALLOW) return 1; if(umi_plugin_extension_host_sandbox_policy_evaluate(&p,1,0,0,0,0)!=UMI_PLUGIN_EXTENSION_HOST_DENY) return 2; return 0; }
