/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/sandbox_policy.h
 *
 * PURPOSE:
 *   Evaluate network, process, workspace-write and external-path sandbox capabilities.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_SANDBOX_POLICY_H
#define UMICOM_PLUGIN_EXTENSION_HOST_SANDBOX_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostSandboxPolicy { int allow_network; int allow_process_spawn; int allow_workspace_write; int allow_external_paths; int allow_environment_read; } UmiPluginExtensionHostSandboxPolicy;
void umi_plugin_extension_host_sandbox_policy_init(UmiPluginExtensionHostSandboxPolicy *policy);
UmiPluginExtensionHostDecision umi_plugin_extension_host_sandbox_policy_evaluate(const UmiPluginExtensionHostSandboxPolicy *policy, int network, int process_spawn, int workspace_write, int external_path, int environment_read);

#ifdef __cplusplus
}
#endif

#endif
