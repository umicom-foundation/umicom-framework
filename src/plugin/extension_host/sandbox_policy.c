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
/*
 * Initialise plugin extension host sandbox policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_sandbox_policy_init(UmiPluginExtensionHostSandboxPolicy *p) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL) { p->allow_network=0; p->allow_process_spawn=0; p->allow_workspace_write=0; p->allow_external_paths=0; p->allow_environment_read=0; } }
/*
 * Provide the plugin extension host sandbox policy evaluate operation used by this module
 * and its client applications.
 */
UmiPluginExtensionHostDecision umi_plugin_extension_host_sandbox_policy_evaluate(const UmiPluginExtensionHostSandboxPolicy *p,int network,int spawn,int write,int external,int env) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL) return UMI_PLUGIN_EXTENSION_HOST_DENY; /* Protect caller-owned memory by checking that required state is available before it is used. */ if((network&&!p->allow_network)||(spawn&&!p->allow_process_spawn)||(external&&!p->allow_external_paths)) return UMI_PLUGIN_EXTENSION_HOST_DENY; /* Protect caller-owned memory by checking that required state is available before it is used. */ if((write&&!p->allow_workspace_write)||(env&&!p->allow_environment_read)) return UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL; return UMI_PLUGIN_EXTENSION_HOST_ALLOW; }
