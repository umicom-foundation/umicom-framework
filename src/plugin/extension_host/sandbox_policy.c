/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/sandbox_policy.c
 *
 * PURPOSE:
 *   Evaluate network, process, workspace-write and external-path sandbox capabilities.
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
#include "umicom/plugin/extension_host/sandbox_policy.h"
void umi_plugin_extension_host_sandbox_policy_init(UmiPluginExtensionHostSandboxPolicy *p) { if(p!=NULL) { p->allow_network=0; p->allow_process_spawn=0; p->allow_workspace_write=0; p->allow_external_paths=0; p->allow_environment_read=0; } }
UmiPluginExtensionHostDecision umi_plugin_extension_host_sandbox_policy_evaluate(const UmiPluginExtensionHostSandboxPolicy *p,int network,int spawn,int write,int external,int env) { if(p==NULL) return UMI_PLUGIN_EXTENSION_HOST_DENY; if((network&&!p->allow_network)||(spawn&&!p->allow_process_spawn)||(external&&!p->allow_external_paths)) return UMI_PLUGIN_EXTENSION_HOST_DENY; if((write&&!p->allow_workspace_write)||(env&&!p->allow_environment_read)) return UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL; return UMI_PLUGIN_EXTENSION_HOST_ALLOW; }
